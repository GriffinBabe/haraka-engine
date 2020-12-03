#pragma once
#include "net/session.hpp"
#include <boost/asio.hpp>
#include <iostream>

namespace net {

/**
 * Server interface is the abstact class of the server that accepts TCP and UDP
 * connections to clients and manages them.
 *
 * To be used, server interface must be inherted with the on_client_connect(),
 * on_client_disconnect() and on_message() implemented.
 */
template <typename EnumType>
class ServerInterface {
public:
    ServerInterface(std::uint16_t port)
        : _acceptor(
            _context,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
    }

    ~ServerInterface()
    {
        stop();
    }

    /**
     * Starts the server by waiting for a new connection and starting a new
     * thread to handle async tasks.
     */
    bool start()
    {
        try {
            wait_for_new_connection();

            _context_thread = std::thread([this]() { _context.run(); });
        }
        catch (std::exception const& exc) {
            std::cerr << "[Server] Exception: " << exc.what() << "\n";
            return false;
        }

        std::cout << "[Server] Started!\n";
        return true;
    }

    /**
     * Stops the server by stopping the context (all the remaining aysnc tasks
     * are skipped) and joining the async task handler thread.
     */
    void stop()
    {
        _context.stop();

        if (_context_thread.joinable()) {
            _context_thread.join();
        }
        std::cout << "[Server] Stopped!\n";
    }

    /**
     * Waits for a new connection, then calls the connect_to_client() method.
     * If connect_to_client() return true, then the client is added to the
     * session queue and calls the connect_to_client() method.
     */
    void wait_for_new_connection()
    {
        _acceptor.async_accept([this](std::error_code ec,
                                      boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                auto session =
                    std::make_shared<TCPSession<EnumType>>(std::move(socket),
                                                        _context,
                                                        _input_queue,
                                                        OwnerType::SERVER);

                if (on_client_connect(session)) {
                    _sessions.push_back(std::move(session));
                    _sessions.back()->connect_to_client(_client_id_counter++);

                    std::cout << "[Server] Connection approved with client: "
                              << _sessions.back()->id() << "\n";
                }
                else {
                    std::cout << "[Server] Connection denied.\n";
                }
            }
            else {
                std::cerr << "[Server] New connection error: " << ec.message()
                          << std::endl;
            }
            wait_for_new_connection();
        });
    }

    /**
     * Sends a message to the client, removes it from the queue if it is
     * disconnected.
     */
    void message_client(std::shared_ptr<net::TCPSession<EnumType>> client,
                        net::Packet<EnumType> const& packet)
    {
        if (client && client->is_connected()) {
            client->send_packet(packet);
        }
        else {
            on_client_disconnect(client);
            client.reset();
            _sessions.erase(
                std::remove(_sessions.begin(), _sessions.end(), client),
                _sessions.end());
        }
    }

    /**
     * Sends a message to all the clients excepts one if specified.
     * (used for broadcasting)
     */
    void message_all_clients(
        net::Packet<EnumType> const& msg,
        std::shared_ptr<net::TCPSession<EnumType>> exclude = nullptr)
    {
        bool invalid_session_exits = false;
        // do the same as in message_client but erases the client only AFTER
        // the iterations, as we don't want to modify AND iterate the queue
        // at the same time.
        for (auto& client : _sessions) {
            if (client && client->is_connected()) {
                if (client != exclude) {
                    client->send_packet(msg);
                }
            }
            else {
                on_client_disconnect(client);
                client.reset();
                invalid_session_exits = true;
            }
        }

        if (invalid_session_exits) {
            _sessions.erase(
                std::remove(_sessions.begin(), _sessions.end(), nullptr),
                _sessions.end());
        }
    }

    /**
     * Reads all the received packets and launches the on_message on each of
     * them. This function is called by the game loop.
     *
     * @param max_messages
     * @param wait
     */
    void update(size_t max_messages = -1, bool wait = false)
    {
        if (wait) {
            _input_queue.wait();
        }

        size_t message_count = 0;

        while (message_count < max_messages && !_input_queue.empty()) {
            auto msg = _input_queue.pop_front();
            on_message(msg.remote, msg.packet);
            message_count++;
        }
    }

protected:
    /**
     * Called upon a client connection. Must be overridden.
     * @param client, pointer to the client's session.
     */
    virtual bool
    on_client_connect(std::shared_ptr<net::TCPSession<EnumType>> client) = 0;

    /**
     * Called upon a client disconnection. Must be overridden.
     * @param client, pointer to the client's session.
     */
    virtual void
    on_client_disconnect(std::shared_ptr<net::TCPSession<EnumType>> client) = 0;

    /**
     * Called when receiving a packet from a client.
     */
    virtual void on_message(std::shared_ptr<net::TCPSession<EnumType>> client,
                            net::Packet<EnumType> packet) = 0;

private:
    /**
     * Boost asio core class for async task handling.
     */
    boost::asio::io_context _context;

    /**
     * Working thread that will asynchronously do the tasks.
     */
    std::thread _context_thread;

    /**
     * Acceptor to accept and establish new TCP sessions.
     */
    boost::asio::ip::tcp::acceptor _acceptor;

    /**
     * Thread safe queue of all the received packets to handle.
     */
    net::ThreadSafeDeque<net::OwnedPacket<EnumType>> _input_queue;

    /**
     * Queue of user sessions.
     */
    std::deque<std::shared_ptr<net::TCPSession<EnumType>>> _sessions;

    /**
     * Client counter.
     */
    std::uint32_t _client_id_counter = 0;
};
} // namespace net
