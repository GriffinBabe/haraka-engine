#include <gtest/gtest.h>
#include <server/server_controller.hpp>
#include <client/client_controller.hpp>

class TestServerClient : public ::testing::Test {
};

TEST_F(TestServerClient, test_connection)
{
    // launches a simple server
    const std::uint16_t port = 2049;
    const std::uint32_t tick_rate = 15;

    server::ServerController server(tick_rate, port);

    // launches a cient and connect to the server
    const std::string hostname = "127.0.0.1";

    client::ClientController client;
    bool success = client.connect(hostname, port);

    ASSERT_TRUE(success);
}
