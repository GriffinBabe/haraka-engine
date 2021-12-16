#pragma once
#include "types.hpp"

namespace core {

class Team : public PrimitiveValue<int> {

public:
    enum TeamType { BLUE=0, RED=1 };

    Team(TeamType type) : PrimitiveValue<int>(type) {}


    bool is_same(Team const& other) const
    {
        return _value == other._value;
    }

};
} // namespace core
