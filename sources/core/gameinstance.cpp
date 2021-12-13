#include "core/gameinstance.hpp"
#include "core/exception.hpp"

core::GameInstance::GameInstance(core::Snapshot& snapshot,
                                 bool server_side,
                                 std::uint32_t tick_rate)
    : _base_snapshot(snapshot),
      _current_snapshot(_base_snapshot),
      _server_side(server_side),
      _tick_rate(tick_rate)

{
    _initialize();
}

std::shared_ptr<core::DeltaSnapshot> core::GameInstance::update_tick()
{
    // creates a new world state
    core::Snapshot next_snapshot(_current_snapshot);

    // fires all the actions in the world state
    while (!_action_queue.empty()) {
        auto action_ptr = _action_queue.pop_front();
        _play_action(action_ptr, next_snapshot);
    }

    // updates physics...
    next_snapshot.update(_ms_per_tick);

    // computes delta snapshot
    core::DeltaSnapshot delta_snapshot(_current_snapshot.tick(),
                                       next_snapshot.tick());
    delta_snapshot.evaluate(_current_snapshot, next_snapshot);

    auto delta_snapshot_ptr =
        std::make_shared<DeltaSnapshot>(std::move(delta_snapshot));

    _delta_snapshots.push_back(delta_snapshot_ptr);
    _current_snapshot = next_snapshot;

    return delta_snapshot_ptr;
}

void core::GameInstance::add_action(std::shared_ptr<core::GameAction> action)
{
    _action_queue.push_back(action);
}

void core::GameInstance::_play_action(std::shared_ptr<core::GameAction> action,
                                      core::Snapshot& snapshot)
{
    _action_status_list.clear();

    ActionStatus status;
    status.action = action;

    try {
        action->act(snapshot);
        status.success = true;
    }
    catch (core::UnknownIDException const& exc) {
        status.success = false;
        status.message = exc.what();
    }
    catch (core::ImpossibleActionException const& exc) {
        status.success = false;
        status.message = exc.what();
    }
    _action_status_list.push_back(status);
}

void core::GameInstance::_initialize()
{
    _ms_per_tick = 1000.0f / (float) _tick_rate;
    _current_snapshot = _base_snapshot;
}

std::vector<core::ActionStatus> core::GameInstance::action_status_list()
{
    return _action_status_list;
}
