#include <vector>

namespace net
{

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

}