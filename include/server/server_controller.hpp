#pragma once

#include "core/gameinstance.hpp"
#include "net/server.hpp"

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
 * Pre-declaration
 */
class HarakaServerInterface;

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
    core::GameInstance _instance;
    std::uint32_t _current_tick = 0;

    const std::uint32_t _tick_rate = 15;
    const float _ms_per_tick = 1000.0f/15.0f;
};


} // namespace server
