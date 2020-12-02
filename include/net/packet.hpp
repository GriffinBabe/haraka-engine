#include <vector>
#include "net/session.hpp"

namespace net
{

/**
 * Forward declare the class.
 */
template <typename EnumType>
class Session;

template <typename EnumType>
struct PacketHeader {
    EnumType id{};

    std::uint32_t size = 0;

};

template <typename EnumType>
struct Packet {

    // Packet head
    PacketHeader<EnumType> header{};

    // Packet content
    std::vector<uint8_t> body;

    size_t size() {
        return body.size();
    }
};

template <typename EnumType>
struct OwnedPacket {

    Packet<EnumType> packet;
    std::shared_ptr<net::Session<EnumType>> remote = nullptr;
};

}