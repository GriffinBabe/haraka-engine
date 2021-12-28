#include "server/session_info.hpp"

server::SessionInfo::SessionInfo()
{
}

bool server::SessionInfo::log(std::string username, std::string token)
{
    // TODO later: contact main api
    _username = username;
    _token = token;
    _status = LOGGED;
}
