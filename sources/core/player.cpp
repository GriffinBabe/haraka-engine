#include "core/player.hpp"

core::Player::Player(std::uint32_t id, core::Team& team)
    : _player_id(id), _team(team)
{

}

std::uint32_t core::Player::player_id() const
{
    return _player_id;
}

core::Team& core::Player::team()
{
    return _team;
}
