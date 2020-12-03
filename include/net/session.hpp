#pragma once
#include "net/packet.hpp"
#include "net/tsdeque.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <variant>

namespace net {

typedef boost::asio::ip::tcp::socket tcp_socket_t;
typedef boost::asio::io_context context_t;

/**
 * Enumeration of the owner type (Server if used by the server, Client if by
 * the client)
 */
enum OwnerType : std::uint8_t { SERVER, CLIENT };

/**
 * Session is the main responsible to read packets and send packet from
 * queues. It is used by the server as it used by the client.
 *
 * This class is needs to be implemented for both the TCP and UDP protocols.
 *
 * @tparam EnumType, the enumeration of all the possible packets.
 */

template <typename EnumType>
class Session : public std::enable_shared_from_this<net::Session<EnumType>> {
public:
    Session(context_t& context,
            net::ThreadSafeDeque<OwnedPacket<EnumType>>& in_queue,
            OwnerType owner_type)
        : _context(context), _input_queue(in_queue), _owner(owner_type)
    {
    }

    /**
     * Adds a packet to the output queue and starts the asynchronous operation
     * of sending the messages if needed.
     */
    virtual void send_packet(const net::Packet<EnumType>& packet) = 0;

    /**
     * Checks if the socket is open and starts read the headers.
     * @param id, the id of this session.
     */
    virtual void connect_to_client(std::uint32_t id) = 0;

    /**
     * Connects to tge server asynchronously and starts to read the packets.
     */
    virtual void connect_to_server(
        boost::asio::ip::tcp::resolver::results_type const& endpoint) = 0;

    /**
     * Returns true if the socket is open, false otherwhise.
     */
    virtual bool is_connected() = 0;

    /**
     * Adds the disconnect order to the context queue.
     */
    virtual void disconnect() = 0;

    /**
     * Returns true if this session's protocol is TCP
     */
    virtual bool is_tcp() = 0;

    /**
     * Returns true if this session's protocol is UDP
     */
    virtual bool is_udp() = 0;

    std::uint32_t id()
    {
        return _id;
    }

protected:
    std::uint32_t _id = 0;

    OwnerType _owner = SERVER;

    /**
     * Reference to the input queue. (Received messages are put to the queue).
     */
    net::ThreadSafeDeque<net::OwnedPacket<EnumType>>& _input_queue;

    /**
     * TCPSession's output queue. (Message to send are put to the queue).
     */
    net::ThreadSafeDeque<net::Packet<EnumType>> _output_queue;

    /**
     * Temporary packet used in the read_header(), read_body() and
     * add_packet_to_input_queue() methods.
     */
    net::Packet<EnumType> _temp_packet;

    /**
     * Reference to the context.
     */
    boost::asio::io_context& _context;

private:
};

/**
 * This class is the implementation of the session for the TCP protocol.
 */
template <typename EnumType>
class TCPSession : public Session<EnumType> {
public:
    TCPSession(tcp_socket_t socket,
               context_t& context,
               net::ThreadSafeDeque<OwnedPacket<EnumType>>& in_queue,
               OwnerType owner_type)
        : Session<EnumType>(context, in_queue, owner_type),
          _socket(std::move(socket))
    {
    }

    void send_packet(const net::Packet<EnumType>& packet) override
    {
        this->_context.post([this, packet] {
            // If the queue is not empty, we assume that
            // we are already asynchronously writing to the network, and adding
            // a new packet to the queue will be processed after the current
            // wrote packet.

            // If the queue is empty, then this means that the async writing
            // process already finished and need to be relaunched.
            bool writing_message = !this->_output_queue.empty();
            this->_output_queue.push_back(packet);
            if (!writing_message) {
                write_header();
            }
        });
    }

    void connect_to_client(std::uint32_t id) override
    {
        if (this->_owner == SERVER) {
            if (is_connected()) {
                this->_id = id;
                read_header();
            }
        }
    }

    void connect_to_server(
        boost::asio::ip::tcp::resolver::results_type const& endpoint) override
    {
        if (this->_owner == CLIENT) {
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

    bool is_connected() override
    {
        return _socket.is_open();
    }

    void disconnect() override
    {
        if (is_connected()) {
            this->_context.post([this]() { close_socket(); });
        }
    }

    bool is_tcp() override
    {
        return true;
    }

    bool is_udp() override
    {
        return false;
    }

private:

    /**
     * Overrides the shared_from_this of the base function by adding a dynamic
     * pointer cast.
     * @return a shared_ptr down-casted to TCPSession.
     */
    std::shared_ptr<TCPSession<EnumType>> shared_from_this()
    {
        return std::dynamic_pointer_cast<TCPSession<EnumType>>(
            net::Session<EnumType>::shared_from_this());
    }

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
            boost::asio::buffer(&this->_output_queue.front().header,
                                sizeof(net::PacketHeader<EnumType>)),
            [this](std::error_code ec, size_t wrote) {
                if (!ec) {
                    if (this->_output_queue.front().body.size() > 0) {
                        write_body();
                    }
                    else {
                        this->_output_queue.pop_front();
                        if (!this->_output_queue.empty()) {
                            write_header();
                        }
                    }
                }
                else {
                    std::cerr << "[TCPSession " << this->_id
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
            boost::asio::buffer(this->_output_queue.front().body.data(),
                                this->_output_queue.front().body.size()),
            [this](std::error_code ec, size_t length) {
                if (!ec) {
                    this->_output_queue.pop_front();
                    if (!this->_output_queue.empty()) {
                        write_header();
                    }
                }
                else {
                    std::cerr << "[TCPSession " << this->_id
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
            boost::asio::buffer(&this->_temp_packet.header,
                                sizeof(PacketHeader<EnumType>)),
            [this](std::error_code ec, size_t length) {
                if (!ec) {
                    if (length > 0) {
                        // Now that we have the size we resize the array of our
                        // packet and proceed to read the body
                        this->_temp_packet.body.resize(
                            this->_temp_packet.header.size);
                        read_body();
                    }
                    else {
                        // length <= 0 means that the packet is header only
                        // we proceed
                        add_packet_to_input_queue();
                    }
                }
                else {
                    std::cout << "[TCPSession " << this->_id
                              << "] Read header failed: " << ec.message()
                              << "\n";
                    close_socket();
                }
            });
    }

    /**
     * Reads the body of the next packet. This method is called by read_header()
     * if necessary.
     */
    void read_body()
    {
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(this->_temp_packet.body.data(),
                                this->_temp_packet.body.size()),
            [this](std::error_code ec, size_t length) {
                if (!ec) {
                    add_packet_to_input_queue();
                }
                else {
                    std::cerr << "[TCPSession " << this->_id
                              << "] Read body failed: " << ec.message() << "\n";
                    close_socket();
                }
            });
    }

    /**
     * Closes the used UDP / TCP socket.
     */
    void close_socket()
    {
        _socket.close();
    }

    /**
     * Adds a packet to the input queue and proceeds by calling read_header()
     */
    void add_packet_to_input_queue()
    {
        if (this->_owner == SERVER) {
            this->_input_queue.push_back(
                {shared_from_this(), this->_temp_packet});
        }
        else {
            this->_input_queue.push_back({nullptr, this->_temp_packet});
        }
        read_header();
    }

    /**
     * The used socket.
     */
    tcp_socket_t _socket;
};
} // namespace net
