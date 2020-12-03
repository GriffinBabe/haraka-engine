#include "net/client.hpp"
#include "net/server.hpp"
#include "protobufs/dummy_packets.pb.h"
#include <gtest/gtest.h>

class TestServer : public ::testing::Test {
};

enum DummyPackets : std::uint32_t {
    CONNECTION = 0,
    DISCONNECTION,
    PING,
    CONNECTION_RESPONSE,
};

class DummyServer : public net::ServerInterface<DummyPackets> {
public:
    DummyServer(std::uint16_t port) : ServerInterface<DummyPackets>(port)
    {
    }

    bool packet_received()
    {
        return _packed_received;
    }

    std::vector<std::string>& usernames()
    {
        return _usernames;
    }

protected:
    bool on_client_connect(
        std::shared_ptr<net::TCPSession<DummyPackets>> client) override
    {
        std::cout << "[Server] On client connect called. \n";
        return true;
    }

    void on_client_disconnect(
        std::shared_ptr<net::TCPSession<DummyPackets>> client) override
    {
        std::cout << "[Server] On client disconnected called. \n";
    }

    void on_message(std::shared_ptr<net::TCPSession<DummyPackets>> client,
                    net::Packet<DummyPackets> packet) override
    {
        std::cout << "[Server] Packet received with packed id:  "
                  << packet.header.id << "\n";
        _packed_received = true;

        switch (packet.header.id) {
        case CONNECTION:
            net::Connection packet_pb;
            packet_pb.ParseFromArray(packet.body.data(), packet.body.size());
            std::cout << "[Server] Client connected with username: "
                      << packet_pb.username() << "\n";
            _usernames.push_back(packet_pb.username());

            // prepares the response
            net::ConnectionResponse response_pb;
            response_pb.set_status(net::ConnectionResponse_Status_ACCEPTED);
            const size_t response_size = response_pb.ByteSizeLong();

            net::Packet<DummyPackets> response;
            response.header.id = CONNECTION_RESPONSE;
            response.header.size = response_size;

            // sets the size of the body and serializes the response message
            // into the packet
            response.body.resize(response_size);
            response_pb.SerializeToArray(response.body.data(),
                                         response.body.size());

            client->send_packet(response);
            break;
        }
    }

private:
    std::vector<std::string> _usernames;

    /**
     * Used for testing, if the server has already received a packet or not.
     */
    bool _packed_received = false;
};

TEST_F(TestServer, test_socket_connection)
{
    // Setting the server on port 2049
    DummyServer server(2049);
    server.start();

    net::Client<DummyPackets> client;
    ASSERT_FALSE(client.is_connected());

    // connecting the client to the server
    client.connect("127.0.0.1", 2049);

    ASSERT_TRUE(client.is_connected());

    client.disconnect();

    ASSERT_FALSE(client.is_connected());

    server.stop();
}

TEST_F(TestServer, test_send_message)
{
    DummyServer server(2049);
    server.start();

    net::Client<DummyPackets> client;
    client.connect("127.0.0.1", 2049);

    net::Connection connection_pb;
    connection_pb.set_username("GriffinBabe");

    net::Packet<DummyPackets> connect_packet;
    connect_packet.header.id = DummyPackets::CONNECTION;
    const size_t packet_size = connection_pb.ByteSizeLong();
    connect_packet.header.size = packet_size;
    connect_packet.body.resize(packet_size);
    connection_pb.SerializeToArray(connect_packet.body.data(), packet_size);

    ASSERT_FALSE(server.packet_received());

    client.send(connect_packet);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    server.update();

    // we expect the server to already have received a packet at this point
    ASSERT_TRUE(server.packet_received());
    ASSERT_EQ(server.usernames().at(0), "GriffinBabe");

    client.disconnect();
    server.stop();
}

TEST_F(TestServer, test_send_received_message)
{
    DummyServer server(2049);
    server.start();

    net::Client<DummyPackets> client;
    client.connect("127.0.0.1", 2049);

    net::Connection connection_pb;
    connection_pb.set_username("GriffinBabe");
    const size_t packet_size = connection_pb.ByteSizeLong();

    net::Packet<DummyPackets> connect_packet;
    connect_packet.header.id = DummyPackets::CONNECTION;
    connect_packet.header.size = packet_size;
    connect_packet.body.resize(packet_size);
    connection_pb.SerializeToArray(connect_packet.body.data(), packet_size);

    client.send(connect_packet);

    // The server receives the connect packet and sends back the SERVER_ACCEPT
    // packet to the client.

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    server.update();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    auto& received = client.input_queue();
    auto response = received.pop_front();

    ASSERT_EQ(response.packet.header.id, CONNECTION_RESPONSE);

    net::ConnectionResponse response_pb;
    bool success = response_pb.ParseFromArray(response.packet.body.data(),
                                              response.packet.header.size);
    ASSERT_TRUE(success);
    ASSERT_EQ(response_pb.status(), net::ConnectionResponse_Status_ACCEPTED);

    server.update();
}