#include "client/client_controller.hpp"

client::ClientController::ClientController()
{

}

bool client::ClientController::connect(const std::string& ip,
                                       std::uint16_t port)
{
    auto success = _client.connect(ip, port);
    return success;
}
