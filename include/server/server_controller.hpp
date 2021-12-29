#pragma once

#include "core/gameinstance.hpp"
#include "net/server.hpp"
#include "server/session_info.hpp"
#include "server/serialization/server_serialization.pb.h"

namespace server {

/**
 * Server controller contains the game instance. It controls the game state with
 * actions provided from the network.
 */
class ServerController : public net::ServerInterface<HarakaPackets> {
public:
    ServerController(std::uint32_t tick_rate = 15, std::uint16_t port = 2049);

    ~ServerController();

    void update_tick();

protected:
    bool on_client_connect(
        std::shared_ptr<net::TCPSession<HarakaPackets>> client) override;

    void on_client_disconnect(
        std::shared_ptr<net::TCPSession<HarakaPackets>> client) override;

    void on_message(std::shared_ptr<net::TCPSession<HarakaPackets>> client,
                    net::Packet<HarakaPackets> packet) override;

private:

    /**
     * Messages all the logged clients by checking the session info.
     * @param packet, the packet to send to each logged client.
     * @param exclude, a session that we eventually would like to exclude.
     */
    void _message_all_clients(
        net::Packet<HarakaPackets> const& packet,
        std::shared_ptr<net::TCPSession<HarakaPackets>> exclude = nullptr)
    {
        for (auto& session_info : _session_info) {
            if (session_info.logged()) {
                message_client(session_info.session(), packet);
            }
        }
    }

    void _parse_action_packet(net::Packet<HarakaPackets>& action_packet);

    /**
     * Parses a connection result (containing credential information) check the
     * credentials and adds it to the Session info map.
     * @param connection_result
     */
    void _parse_connection_result(std::shared_ptr<net::TCPSession<HarakaPackets>> client, net::Packet<HarakaPackets>& connection_result);

    /**
     * Sends a delta update to all the clients. This contains all the
     * information such a the delta snapshotn, the actions related to the new
     * tick as well as the action results.
     * @param delta, a DeltaSnapshot object.
     * @param status_list, a list of action status
     * @param actions, a list of GameAction pointers.
     */
    void _send_delta_update(
        core::DeltaSnapshot const& delta,
        std::vector<core::ActionStatus> const& status_list,
        std::vector<std::shared_ptr<core::GameAction>> const& actions);

    /**
     * Encodes the protocol buffer into a packet containing the serialized data
     * as well as the packet type.
     * @param msg, a pointer to the protocol buffer.
     * @param type, the packet type.
     * @return an Haraka Packet.
     */
    net::Packet<HarakaPackets> _encode_packet(google::protobuf::Message* msg,
                                              HarakaPackets type) const;

    /**
     * Decodes a packet and returns the protocol buffer specified as template
     * parameter.
     * @param packet, an haraka packet.
     * @return a deserialized protocol buffer.
     */
    // decodes a packet into a google protobuf type
    template <typename PacketClass>
    PacketClass _decode_packet(net::Packet<HarakaPackets> const& packet)
    {
        static_assert(
            std::is_base_of<google::protobuf::Message, PacketClass>::value,
            "PacketClass must derive from google::protobuf::Message");
        PacketClass buffer;
        buffer.ParseFromArray(packet.body.data(), packet.body.size());
        return buffer;
    }

    /**
     * Reference to GameInstance, containing the game state and snapshots
     */
    core::GameInstance _instance;

    // TODO create TSVector instead for thread safety
    std::vector<SessionInfo> _session_info;

    std::uint32_t _current_tick = 0;
    const std::uint32_t _tick_rate = 15;
    const float _ms_per_tick = 1000.0f / 15.0f;
};

} // namespace server
