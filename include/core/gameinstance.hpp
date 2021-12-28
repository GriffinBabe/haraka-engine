#pragma once

#include "core/action.hpp"
#include "core/events.hpp"
#include "core/player.hpp"
#include "core/snapshot.hpp"
#include "util/tsdeque.hpp"
#include <deque>
#include <vector>

namespace core {
/**
 * Base game class, used both by client and server
 */
class GameInstance {
public:

    /**
     * Default constructor, only useful in other class constructors.
     */
    GameInstance();

    /**
     * Base constructor, should be used to initialize a GameInstance.
     * @param snapshot, base game state can be empty
     * @param server_side, is this instance running on a server? boolean flag
     * @param tick_rate, number of tick simulation per second
     */
    GameInstance(core::Snapshot const snapshot,
                 bool server_side = false,
                 std::uint32_t tick_rate = 15);

    /**
     * Adds an action to the action list. This action will be used in the next
     * tick simulation.
     * @param action, pointer to a GameAction
     */
    void add_action(std::shared_ptr<core::GameAction> action);

    /**
     * Makes a copy of the action queue in a vector and returns it.
     */
    std::vector<std::shared_ptr<core::GameAction>> action_list();

    /**
     * Simulates the next snapshot from the current snapshot.
     * @return A pointer to a delta snapshot. Will be used over the network.
     */
    std::shared_ptr<core::DeltaSnapshot> update_tick();

    /**
     * @return the action status list.
     */
    [[nodiscard]] std::vector<ActionStatus> action_status_list();

    /**
     * @return the current snapshot.
     */
    [[nodiscard]] core::Snapshot const& current_snapshot() const;

private:
    /**
     * Executes an action, performing a change on the game state or return false
     * if the action is impossible. (If the player is attempting to execute
     * something forbidden by the game rules).
     * @param action, a pointer to a GameAction.
     * @param snapshot the snapshot where the actions are performed.
     * @return true if the action succeeded, false otherwise.
     */
    ActionStatus _play_action(std::shared_ptr<core::GameAction> action,
                      Snapshot& snapshot);

    /**
     * Performs initialization, depends if server or client side.
     */
    void _initialize();

    /**
     * The actions waiting to be used in the next snapshot
     */
    util::ThreadSafeDeque<std::shared_ptr<GameAction>> _action_queue;

    /**
     * Contains reports of the action status that where used when computing the
     * current snapshot.
     */
    std::vector<ActionStatus> _action_status_list;

    /**
     * The number of simulations (server-side) done per seconds.
     */
    const std::uint32_t _tick_rate;

    /**
     * Milliseconds between each tick.
     */
    float _ms_per_tick;

    /**
     * Is this a server side instance?
     */
    const bool _server_side;

    /**
     * The initial game state.
     */
    const Snapshot _base_snapshot;

    /**
     * The current game state
     */
    Snapshot _current_snapshot;

    /**
     * A double access queue containing all the delta snapshots of the game.
     */
    std::deque<std::shared_ptr<DeltaSnapshot>> _delta_snapshots;
};

} // namespace core
