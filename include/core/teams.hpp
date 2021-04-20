#pragma once

namespace core {
struct Team {

    enum TeamType { BLUE=0, RED=1 };

    TeamType _type;

    Team(TeamType type) : _type(type) {}

    bool is_same(Team& other)
    {
        return _type == other._type;
    }

};
} // namespace core
