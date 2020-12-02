#pragma once
#include "net/session.hpp"
#include <boost/asio.hpp>

namespace net {
class ServerInterface {
public:
    ServerInterface();

    ~ServerInterface();

protected:
    /**
     * Called upon a client connection. Must be overridden.
     * @param client, pointer to the client's session.
     */
    virtual bool on_client_connect(std::shared_ptr<net::Session> client) = 0;

    /**
     * Called upon a client disconnection. Must be overridden.
     * @param client, pointer to the client's session.
     */
    virtual void on_client_disconnect(std::shared_ptr<net::Session> client) = 0;

    /**
     * Called when receiving a packet from a client.
     */
    virtual void on_message(std::shared_ptr<net::Session> client,
                            void* packet) = 0;

private:
    /**
     * Blocking function that will start all the working threads of the server
     * and then join them once their work is done.
     */
    bool start_threads(std::uint16_t thread_count);

    void worker_thread(std::shared_ptr<boost::asio::io_service> service);

    std::shared_ptr<boost::asio::io_service> _service;
    std::shared_ptr<boost::asio::io_service::work> _work;
};
} // namespace net
