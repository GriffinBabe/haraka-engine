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
    // ask client login
    return true;
}
void server::ServerController::on_client_disconnect(
    std::shared_ptr<net::TCPSession<HarakaPackets>> client)
{
}

void server::ServerController::on_message(
    std::shared_ptr<net::TCPSession<HarakaPackets>> client,
    net::Packet<server::HarakaPackets> packet)
{
}

void server::ServerController::update_tick()
{
    // evaluated delta snapshot
    auto delta = _instance.update_tick();
    auto action_result = _instance.action_status_list();

    // TODO serialize with protocol buffers

}
