// session information such as username, status etc...

#pragma once
#include "net/session.hpp"

namespace server {

enum SessionStatus {
    DISCONNECTED = 0,
    LOGGED = 1;
};

class SessionInfo {
public:
    SessionInfo();

    bool log(std::string username, std::string token);

private:

    SessionStatus _status = DISCONNECTED;

    std::string _username;

    std::string _token;

    std::shared_ptr<net::TCPSession> _session;

};
}