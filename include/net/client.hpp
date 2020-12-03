#pragma once
#include "net/session.hpp"

namespace net {
template <typename EnumType>
class Client {
public:
    Client()
    {
    }

    virtual ~Client()
    {
        disconnect();
    }

    bool connect(const std::string& host, std::uint16_t port)
    {
        try {
            boost::asio::ip::tcp::resolver resolver(_context);
            boost::asio::ip::tcp::resolver::results_type endpoints =
                resolver.resolve(host, std::to_string(port));

            _session = std::make_unique<net::Session<EnumType>>(
                std::move(boost::asio::ip::tcp::socket(_context)),
                _context,
                _input_queue,
                OwnerType::CLIENT);

            _session->connect_to_server(endpoints);

            _context_thread = std::thread([this]() { _context.run(); });
        }
        catch (std::exception const& exc) {
            std::cout << "[Client] Exception while connecting : " << exc.what()
                      << "\n";
            return false;
        }
        return true;
    }

    void disconnect()
    {
        if (is_connected()) {
            _session->disconnect();
        }
        _context.stop();
        if (_context_thread.joinable()) {
            _context_thread.join();
        }
        // Destroys the connection object
        _session.release();
    }

    bool is_connected()
    {
        if (_session) {
            return _session->is_connected();
        }
        return false;
    }

    void send(net::Packet<EnumType> const& msg)
    {
        if (is_connected()) {
            _session->send_packet(msg);
        }
    }

    net::ThreadSafeDeque<net::OwnedPacket<EnumType>>& input_queue()
    {
        return _input_queue;
    }

protected:
    boost::asio::io_context _context;
    std::thread _context_thread;
    std::unique_ptr<net::Session<EnumType>> _session = nullptr;

private:
    net::ThreadSafeDeque<net::OwnedPacket<EnumType>> _input_queue;
};
} // namespace net