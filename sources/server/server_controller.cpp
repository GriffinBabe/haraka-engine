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
    // changes player info
}

void server::ServerController::on_message(
    std::shared_ptr<net::TCPSession<HarakaPackets>> client,
    net::Packet<server::HarakaPackets> packet)
{
    switch(packet.header.id) {
    case CONNECTION_RESULT:
        break;
    case DISCONNECTION:
        break;
    case FULL_SNAPSHOT:
        break;
    case ACTION:
        break;
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
    _message_all_clients(packet);
}
