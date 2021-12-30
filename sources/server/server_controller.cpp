#include "server/server_controller.hpp"

// -----------------------------------------------------------------------------
// Server controller definitions
// -----------------------------------------------------------------------------

server::ServerController::ServerController(std::uint32_t tick_rate,
                                           std::uint16_t port)
    : net::ServerInterface<HarakaPackets>(port),
      _tick_rate(tick_rate),
      _ms_per_tick(1000.0f / ((float) _tick_rate)),
      _instance(core::Snapshot(0), true, tick_rate)
{
}

bool server::ServerController::on_client_connect(
    std::shared_ptr<net::TCPSession<HarakaPackets>> client)
{
    // the server created a TCPSession but here we don't create a session log
    // until the client hasn't given credentials

    // ask for connection information (username, token)
    net::Packet<HarakaPackets> packet;
    packet.header.id = HarakaPackets::CONNECTION;
    packet.header.size = 0;

    message_client(client, packet);
    return true;
}

void server::ServerController::on_client_disconnect(
    std::shared_ptr<net::TCPSession<HarakaPackets>> client)
{
    // checks if there is a session info attached to this client session
    auto it = std::find_if(
        _session_info.begin(), _session_info.end(), [&](auto& val) {
            return std::const_pointer_cast<
                       std::shared_ptr<net::TCPSession<HarakaPackets>>>(
                       val.session())
                       .get()
                   == client.get();
        });
    // disconnect this session if found
    if (it != _session_info.end()) {
        it->disconnect();
    }
}

void server::ServerController::on_message(
    std::shared_ptr<net::TCPSession<HarakaPackets>> client,
    net::Packet<server::HarakaPackets> packet)
{
    // first check if there is a session info attached to this client
    auto it = std::find_if(
        _session_info.begin(), _session_info.end(), [&](const auto& val) {
          return std::const_pointer_cast<
              std::shared_ptr<net::TCPSession<HarakaPackets>>>(
              val.session())
                     .get()
                 == client.get();
        });

    // if there is no session found and that is a connection packet
    if (it == _session_info.end()
        && packet.header.id == HarakaPackets::CONNECTION) {
        _parse_connection_result(client, packet);
    }
    // if there is a session and it is logged
    else if (it->logged()) {
        switch (packet.header.id) {
        case DISCONNECTION:
            break;
        case FULL_SNAPSHOT:
            break;
        case ACTION:
            break;
        }
    }
}

void server::ServerController::update_tick()
{
    // makes a copy of all the instance actions
    auto actions = _instance.action_list();

    // TODO problem: an action might be added here by another thread. FIX

    // evaluated delta snapshot
    auto delta = _instance.update_tick();
    auto action_result = _instance.action_status_list();

    // sends to clients
    _send_delta_update(*delta, action_result, actions);
}

net::Packet<server::HarakaPackets>
server::ServerController::_encode_packet(google::protobuf::Message* msg,
                                         server::HarakaPackets type) const
{
    net::Packet<HarakaPackets> packet;
    packet.header.id = type;

    const auto body_size = msg->ByteSizeLong();
    packet.header.size = body_size;
    packet.body.resize(body_size);

    msg->SerializeToArray(packet.body.data(), body_size);

    return packet;
}

void server::ServerController::_send_delta_update(
    const core::DeltaSnapshot& delta,
    const std::vector<core::ActionStatus>& status_list,
    const std::vector<std::shared_ptr<core::GameAction>>& actions)
{
    server::serialization::DeltaSnapshotUpdate update;

    // prepares the buffer containing all the update information
    auto delta_buffer = delta.serialize();
    update.set_allocated_delta_snapshot(&delta_buffer);

    for (auto const& status : status_list) {
        auto serialized_status = status.serialize();
        auto* buffer_status_ptr = update.add_status_list();
        *buffer_status_ptr = serialized_status;
    }

    for (auto const& action : actions) {
        auto serialized_action = action->serialize();
        auto* buffer_action_ptr = update.add_actions();
        *buffer_action_ptr = serialized_action;
    }

    // encodes the buffer in a packet
    auto packet = _encode_packet(&delta_buffer, server::DELTA_SNAPSHOT_RESULT);

    // sends the packet
    message_all_clients(packet);
}

void server::ServerController::_parse_connection_result(
    std::shared_ptr<net::TCPSession<HarakaPackets>> client,
    net::Packet<HarakaPackets>& connection_result)
{
    // TODO check if session doesn't already exists
    SessionInfo info(client);

    auto connection_buffer =
        _decode_packet<serialization::ConnectionResponse>(connection_result);

    auto logged =
        info.log(connection_buffer.username(), connection_buffer.token());

    if (logged) {
        net::Packet<HarakaPackets> accepted_packet;

        accepted_packet.header.id = HarakaPackets::CONNECTION_CONFIRMED;
        accepted_packet.header.size = 0;

        message_client(client, accepted_packet);

        _session_info.push_back(info);
    }
    else {
        info.disconnect();

        net::Packet<HarakaPackets> denied_packet;
        denied_packet.header.id = HarakaPackets::CONNECTION_DENIED;
        denied_packet.header.size = 0;

        message_client(client, denied_packet);
    }
}
