#pragma once

#include "core/snapshot.hpp"

namespace core {

/**
 * This is the core class of the whole engine. It is responsible for updating
 * the state and create new snapshots.
 */
class Engine {
public:
    Engine();

    /**
     * Applies the user commands as an
     * @return
     */
    bool set_commands();

    /**
     * Updates the physics, changing the current snapshot
     * @return
     */
     bool update_physics();

     std::shared_ptr<DeltaSnapshot> get_delta_snapshot();



private:
    SnapshotTimeLine _timeline;

    Snapshot _current_snapshot;
    Snapshot _base_snapshot;
};

} // namespace core
