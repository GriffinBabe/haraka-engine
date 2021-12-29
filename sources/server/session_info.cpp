#include "server/session_info.hpp"

server::SessionInfo::SessionInfo(
    const std::shared_ptr<net::TCPSession<HarakaPackets>>& session)
    : _session(session)
{
}

bool server::SessionInfo::log(
    std::string username,
    std::string token,
    std::shared_ptr<net::TCPSession<HarakaPackets>> const& reconnection)
{
    // TODO later: contact main api to check credentials
    _username = std::move(username);
    _token = std::move(token);
    _status = LOGGED;

    if (reconnection != nullptr) {
        _session = reconnection;
    }

    return true;
}

bool server::SessionInfo::logged() const
{
    return _status == LOGGED;
}

std::shared_ptr<net::TCPSession<server::HarakaPackets>>
server::SessionInfo::session()
{
    return _session;
}

void server::SessionInfo::disconnect()
{
    _status = DISCONNECTED;
    _session.reset();
}
