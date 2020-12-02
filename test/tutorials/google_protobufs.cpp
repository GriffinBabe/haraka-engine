#include "protobufs/connection.pb.h"
#include <iostream>

namespace gp = google::protobuf;

int main(int argc, char** argv)
{
    // checks if the version of the library corresponds to the version of the
    // buffer generated classes
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::string serialized;

    {
        // connection packet initialized
        tutorial::Connection connection;

        assert(!connection.IsInitialized());

        connection.set_password("NiceTryWebScraper");
        connection.set_instance(0);

        assert(!connection.IsInitialized());

        connection.set_username("GriffinBabe");

        bool success = connection.SerializeToString(&serialized);

        assert(success);

        std::cout << serialized << std::endl;
    }

    std::unique_ptr<gp::Message> message =
        std::make_unique<tutorial::Connection>();

    bool success = message->ParseFromString(serialized);
    assert(success);

    auto connection = dynamic_cast<tutorial::Connection*>(message.get());
    assert(connection != nullptr);

    assert(connection->username() == "GriffinBabe");
    assert(connection->instance() == 0);
    assert(connection->password() == "NiceTryWebScraper");

    // frees all the protobuf objects
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}