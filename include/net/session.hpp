#pragma once
#include "net/packet.hpp"
#include "net/tsdeque.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <variant>

namespace net {

typedef boost::asio::ip::tcp::socket tcp_socket_t;
typedef boost::asio::ip::udp::socket udp_socket_t;
typedef boost::asio::io_context context_t;

/**
 * Socket's protocol type.
 */
enum SocketType : std::uint8_t { TCP = 0, UDP };

/**
 * Session is the main responsible to read packets and send packet from queues.
 * It is used by the server as it used for the client.
 *
 * The session supports both the UDP and the TCP protocols, the server creates
 * a session for each for each connected client, as the engine uses both
 * protocols.
 *
 * @tparam EnumType, the enumeration of all the possible packets.
 */
template <typename EnumType>
class Session : public std::enable_shared_from_this<net::Session<EnumType>> {
public:
    /**
     * Enumeration of the owner type (Server if used by the server, Client if by
     * the client)
     */
    enum OwnerType : std::uint8_t { SERVER, CLIENT };

    Session(tcp_socket_t socket,
            context_t& context,
            net::ThreadSafeDeque<OwnedPacket<EnumType>>& in_queue,
            OwnerType owner_type)
        : _type(TCP),
          _socket(std::move(socket)),
          _context(context),
          _owner(owner_type),
          _input_queue(in_queue)
    {
    }

    Session(udp_socket_t socket,
            context_t& context,
            net::ThreadSafeDeque<OwnedPacket<EnumType>>& in_queue,
            OwnerType owner_type)
        : _type(UDP),
          _socket(std::move(socket)),
          _context(context),
          _owner(owner_type),
          _input_queue(in_queue)
    {
    }

    /**
     * Adds a packet to the output queue and starts the asynchronous operation
     * of sending the messages if needed.
     */
    void send_packet(const net::Packet<EnumType>& packet)
    {
        _context.post([this, packet] {
            // If the queue is not empty, we assume that
            // we are already asynchronously writing to the network, and adding
            // a new packet to the queue will be processed after the current
            // wrote packet.

            // If the queue is empty, then this means that the async writing
            // process already finished and need to be relaunched.
            bool writing_message = !_output_queue.empty();
            _output_queue.push_back(packet);
            if (!writing_message) {
                write_header();
            }
        });
    }

    /**
     * Checks if the socket is open and starts read the headers.
     * @param id, the id of this session.
     */
    void connect_to_client(std::uint32_t id)
    {
        if (_owner == SERVER) {
            if (is_connected()) {
                _id = id;
                read_header();
            }
        }
    }

    /**
     * Connects to tge server asynchronously and starts to read the packets.
     */
    void connect_to_server(
        boost::asio::ip::tcp::resolver::results_type const& endpoint)
    {
        if (_owner == CLIENT) {
            boost::asio::async_connect(
                _socket,
                endpoint,
                [this](std::error_code ec,
                       boost::asio::ip::tcp::endpoint endpoint) {
                    if (!ec) {
                        read_header();
                    }
                });
        }
    }

    /**
     * Returns true if the socket is open, false otherwhise.
     */
    bool is_connected()
    {
        if (_type == TCP) {
            return std::get<tcp_socket_t>(_socket).is_open();
        }
        else if (_type == UDP) {
            return std::get<udp_socket_t>(_socket).is_open();
        }
        return false;
    }

    /**
     * Adds the disconnect order to the context queue.
     */
    void disconnect()
    {
        if (is_connected()) {
            _context.post([this]() {
                close_socket();
            });
        }
    }

    /**
     * Returns true if this session's protocol is TCP
     */
    bool is_tcp()
    {
        return _type == TCP;
    }

    /**
     * Returns true if this session's protocol is UDP
     */
    bool is_udp()
    {
        return _type == UDP;
    }

    std::uint32_t id()
    {
        return _id;
    }

private:
    /**
     * Writes the output queue's front packet's header. If necessary, calls the
     * write_body() method. If not necessary then pops it from the output queue,
     * and, if the output queue is not empty, then proceeds by calling again
     * write_header().
     */
    void write_header()
    {
        boost::asio::async_write(
            _socket,
            boost::asio::buffer(_output_queue.front().header,
                                sizeof(net::Packet<EnumType>)),
            [this](std::error_code ec, size_t wrote) {
                if (!ec) {
                    if (_output_queue.front().body.size() > 0) {
                        write_body();
                    }
                    else {
                        _output_queue.pop_front();
                        if (!_output_queue.empty()) {
                            write_header();
                        }
                    }
                }
                else {
                    std::cerr << "[Session " << _id
                              << "] Write header failed: " << ec.message()
                              << "\n";
                    close_socket();
                }
            });
    }

    /**
     * Writes the output queue's front packet's body and pops it from the queue,
     * then calls the write_header() method if the output queue is still not
     * empty.
     */
    void write_body()
    {
        boost::asio::async_write(
            _socket,
            boost::asio::buffer(_output_queue.front().body.data(),
                                _output_queue.front().body.size()),
            [this](std::error_code ec, size_t length) {
                if (!ec) {
                    _output_queue.pop_front();
                    if (!_output_queue.empty()) {
                        write_header();
                    }
                }
                else {
                    std::cerr << "[Session " << _id
                              << "] Write body failed: " << ec.message()
                              << "\n";
                    close_socket();
                }
            });
    }

    /**
     * Reads the header of the next packet and sets the length of the temporary
     * packet body accordingly.
     */
    void read_header()
    {
        // this shouldn't compile
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(&_temp_packet.header,
                                sizeof(PacketHeader<EnumType>)),
            [this](std::error_code ec, size_t length) {
                if (!ec) {
                    if (length > 0) {
                        // Now that we have the size we resize the array of our
                        // packet and proceed to read the body
                        _temp_packet.body.resize(_temp_packet.header.size);
                        read_body();
                    }
                    else {
                        // length <= 0 means that the packet is header only
                        // we proceed
                    }
                }
                else {
                    std::cout << "[Session " << _id
                              << "] Read header failed: " << ec.message()
                              << "\n";
                }
                close_socket();
            });
    }

    /**
     * Reads the body of the next packet. This method is called by read_header()
     * if necessary.
     */
    void read_body()
    {
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_temp_packet.body.data(),
                                                    _temp_packet.body.size()),
                                [this](std::error_code ec, size_t length) {
                                    if (!ec) {
                                        add_packet_to_input_queue();
                                    }
                                    else {
                                        std::cerr << "[Session " << _id
                                                  << "] Read body failed: "
                                                  << ec.message() << "\n";
                                        close_socket();
                                    }
                                });
    }

    /**
     * Closes the used UDP / TCP socket.
     */
    void close_socket()
    {
        if (_type == TCP) {
            std::get<tcp_socket_t>(_socket).close();
        }
        else if (_type == UDP) {
            std::get<udp_socket_t>(_socket).close();
        }
    }

    /**
     * Adds a packet to the input queue and proceeds by calling read_header()
     */
    void add_packet_to_input_queue()
    {
        if (_owner == SERVER) {
            _input_queue.push_back({this->shared_from_this(), _temp_packet});
        }
        else {
            _input_queue.push_back({nullptr, _temp_packet});
        }
        read_header();
    }

    /**
     * Temporary packet used in the read_header(), read_body() and
     * add_packet_to_input_queue() methods.
     */
    net::Packet<EnumType> _temp_packet;

    /**
     * The used socket.
     */
    std::variant<tcp_socket_t, udp_socket_t> _socket;

    /**
     * Reference to the context.
     */
    boost::asio::io_context& _context;

    /**
     * Reference to the input queue. (Received messages are put to the queue).
     */
    net::ThreadSafeDeque<net::OwnedPacket<EnumType>>& _input_queue;

    /**
     * Session's output queue. (Message to send are put to the queue).
     */
    net::ThreadSafeDeque<net::Packet<EnumType>> _output_queue;

    /**
     * The protocol type of the socket.
     */
    SocketType _type = TCP;

    /**
     * The owner type (server/client)
     */
    OwnerType _owner = SERVER;

    /**
     * The ID of the session.
     */
    std::uint32_t _id = 0;
};
} // namespace net