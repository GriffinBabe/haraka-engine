#pragma once
#include "net/session.hpp"

namespace net {
template <typename EnumType>
class ClientInterface {
public:
    ClientInterface()
    {
    }

    virtual ~ClientInterface()
    {
        disconnect();
    }

    bool connect(const std::string& host, std::uint16_t port)
    {
        try {
            boost::asio::ip::tcp::resolver resolver(_context);
            boost::asio::ip::tcp::resolver::results_type endpoints =
                resolver.resolve(host, std::to_string(port));

            
        }
    }

    void disconnect()
    {
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
    }

    net::ThreadSafeDeque<net::Packet<EnumType>>& input_queue()
    {
        return _input_queue;
    }

protected:
    boost::asio::io_context _context;
    std::thread _context_thread;
    std::unique_ptr<net::Session<EnumType>> _session = nullptr;

private:
    net::ThreadSafeDeque<net::Packet<EnumType>> _input_queue;
};
} // namespace net