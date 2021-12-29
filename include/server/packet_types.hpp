#pragma once
#include <cstdint>

namespace server {

enum HarakaPackets : std::uint32_t {
    CONNECTION = 0,            // sent by server upon connection
    CONNECTION_RESULT = 1,     // response from user after connection packet
    CONNECTION_CONFIRMED = 2,  // response sent by server when connection is ok
    CONNECTION_DENIED = 3,     // response sent by server when connection is denied
    DISCONNECTION = 4,         // sent when user is leaving
    FULL_SNAPSHOT = 5,         // user requesting a full snapshot update
    FULL_SNAPSHOT_RESULT = 6,  // response containing full snapshot update
    DELTA_SNAPSHOT_RESULT = 7, // sent by server at each new tick
    ACTION = 8,                // sent by user when making an action (changing direction, firing, spells)
};

}
