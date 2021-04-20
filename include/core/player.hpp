#pragma once

#include "core/teams.hpp"
#include <cinttypes>

namespace core {

class Player {
public:
    Player(std::uint32_t id, core::Team& team);

    [[nodiscard]] inline std::uint32_t player_id() const;

    core::Team& team();

private:
    core::Team _team;

    std::uint32_t _player_id;
};

} // namespace core
