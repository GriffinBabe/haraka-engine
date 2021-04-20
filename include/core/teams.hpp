#pragma once

namespace core {
struct Team {

    enum TeamType { BLUE=0, RED=1 };

    TeamType type;

    bool is_same(Team& other)
    {
        return type == other.type;
    }

};
} // namespace core
