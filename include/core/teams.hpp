#pragma once
#include "types.hpp"

namespace core {
struct Team : PrimitiveValue<int> {

    enum TeamType { BLUE=0, RED=1 };

    Team(TeamType type) : PrimitiveValue<int>(type) {}

    bool is_same(Team& other)
    {
        return _value == other._value;
    }

};
} // namespace core
