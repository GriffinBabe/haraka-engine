#pragma once
#include <cstdint>
#include <memory>
#include <vector>

namespace net {

/**
 * Forward declare the class.
 */
template <typename EnumType>
class TCPSession;

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

    size_t size()
    {
        return body.size();
    }
};

template <typename EnumType>
struct OwnedPacket {
    std::shared_ptr<net::TCPSession<EnumType>> remote = nullptr;
    Packet<EnumType> packet;
};

} // namespace net
