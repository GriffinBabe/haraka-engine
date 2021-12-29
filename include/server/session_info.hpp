// session information such as username, status etc...

#pragma once
#include "net/session.hpp"
#include "server/packet_types.hpp"

namespace server {

enum SessionStatus { DISCONNECTED = 0, LOGGED = 1 };

class SessionInfo {
public:
    SessionInfo(std::shared_ptr<net::TCPSession<HarakaPackets>> const& session);

    /**
     * Logs the session. Contacting the main server API to check credentials.
     * If the credentials are good,
     * @param username, the username of the player
     * @param token, a authentication token obtained from the API.
     * @param reconnection, if the client has reconnected, then sets the session.
     * @return true if the logging was successful, false otherwise.
     */
    bool log(std::string username,
             std::string token,
             std::shared_ptr<net::TCPSession<HarakaPackets>> const&
                 reconnection = nullptr);

    void disconnect();

    [[nodiscard]] bool logged() const;

    [[nodiscard]] std::shared_ptr<net::TCPSession<HarakaPackets>> session();

private:
    SessionStatus _status = DISCONNECTED;

    std::string _username;

    std::string _token;

    // Pointer to the TCP session. !! Might be reset when the client disconnects
    std::shared_ptr<net::TCPSession<HarakaPackets>> _session;
};
} // namespace server