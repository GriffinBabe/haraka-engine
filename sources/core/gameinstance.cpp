#include "core/gameinstance.hpp"
#include "core/exception.hpp"

core::GameInstance::GameInstance()
    : _server_side(false),
      _tick_rate(0),
      _base_snapshot(core::Snapshot(0)),
      _current_snapshot(_base_snapshot)
{
    // unimplemented
}

core::GameInstance::GameInstance(core::Snapshot const snapshot,
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
    _action_status_list.clear();
    while (!_action_queue.empty()) {
        auto action_ptr = _action_queue.pop_front();
        auto status = _play_action(action_ptr, next_snapshot);
        _action_status_list.push_back(status);
    }

    // updates physics, delta time in seconds
    next_snapshot.update(_ms_per_tick / 1000.0f);

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

core::ActionStatus
core::GameInstance::_play_action(std::shared_ptr<core::GameAction> action,
                                 core::Snapshot& snapshot)
{
    ActionStatus status;
    status.action_id = action->id();

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
    return status;
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

core::Snapshot const& core::GameInstance::current_snapshot() const
{
    return _current_snapshot;
}

std::vector<std::shared_ptr<core::GameAction>> core::GameInstance::action_list()
{
    return _action_queue.to_vector();
}
