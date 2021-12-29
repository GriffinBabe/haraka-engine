#pragma once
#include "net/client.hpp"
#include "server/packet_types.hpp"
#include "server/session_info.hpp"

namespace client {

class ClientController {
public:
    ClientController();

    bool connect(std::string const& ip, std::uint16_t port);

protected:

private:

    server::SessionStatus _status = server::DISCONNECTED;
    net::Client<server::HarakaPackets> _client;
};

}
