#pragma once

#include "core/gameinstance.hpp"
#include "net/server.hpp"
#include "server/serialization/server_serialization.pb.h"

namespace server {

enum HarakaPackets : std::uint32_t {
    CONNECTION = 0,            // sent by server upon connection
    CONNECTION_RESULT = 1,     // response from user after connection packet
    DISCONNECTION = 2,         // sent when user is leaving
    FULL_SNAPSHOT = 3,         // user requesting a full snapshot update
    FULL_SNAPSHOT_RESULT = 4,  // response containing full snapshot update
    DELTA_SNAPSHOT_RESULT = 5, // sent by server at each new tick
    ACTION = 6, // sent by user when making an action (changing direction,
                // firing, spells)
};

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

    // boilerplate redefinition to avoid template problems
    void _message_all_clients(
        net::Packet<HarakaPackets> const& packet,
        std::shared_ptr<net::TCPSession<HarakaPackets>> exclude = nullptr)
    {
        message_all_clients(packet, std::move(exclude));
    }

    // boilerplate redefinition to avoid template problems
    void _message_client(std::shared_ptr<net::TCPSession<HarakaPackets>> client,
                         net::Packet<HarakaPackets> const& packet)
    {
        message_client(std::move(client), packet);
    }

    void _parse_action_packet(net::Packet<HarakaPackets>& action_packet);

    void _parse_connection_result(net::Packet<HarakaPackets>& connection_result);

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

    core::GameInstance _instance;

    std::uint32_t _current_tick = 0;
    const std::uint32_t _tick_rate = 15;
    const float _ms_per_tick = 1000.0f / 15.0f;
};

} // namespace server
