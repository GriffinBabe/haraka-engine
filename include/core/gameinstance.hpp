#pragma once

#include <vector>
#include "core/world.hpp"
#include "core/action.hpp"
#include "core/player.hpp"
#include "core/events.hpp"

namespace core {
/**
 * Base game class, used both by client and server
 */
class GameInstance {

public:

    GameInstance(core::World& world);

    /**
     * Adds a new player to the player list.
     * @param player
     */
    void add_player(core::Player const& player);

    /**
     * Play an action on a GameObject. Generally launched from a packets.
     * @param action
     * @return
     */
    bool play_action(std::shared_ptr<core::GameAction> action);

private:
    core::World& _world;

    std::vector<core::Player> _players;
};

}
