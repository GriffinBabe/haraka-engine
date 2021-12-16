#pragma once
#include "core/game_object.hpp"
#include <map>

namespace core {

/**
 * Contains all the differences between all the objects of two snapshots.
 *
 * std::uint32_t is the id of the object in the snapshot.
 * std::string is the name of the variables
 * core::value_t is the delta difference.
 */
typedef std::map<std::uint32_t, diffset_t> diffmap_t;

typedef std::map<std::uint32_t, std::shared_ptr<GameObject>> object_map_t;

// forward declaration
class DeltaSnapshot;

/**
 * A snapshot contains all game state information at a giving time.
 */
class Snapshot : std::enable_shared_from_this<Snapshot> {
    friend class DeltaSnapshot;

public:
    Snapshot(std::uint32_t tick);

    /**
     * Copy constructor. Will copy all the data in the object. The tick is by
     * default incremented by one.
     */
    Snapshot(Snapshot const& other);

    /**
     * Simulates all the game physics
     * @param delta_time, the time between the two snapshots.
     */
    void update(float delta_time);

    /**
     * Applies a delta snapshot, creating a new (interpolated) snapshot.
     * @param delta, the delta snapshot
     * @param interp, interpolation time (1.0 being the maximum and 0.0 the
     * minimum). An interpolation time of 1.0 will result in a full apply of the
     * delta snapshot. Objects are added and removed at the 1.0 interpolation
     * time, not before.
     * @return a new interpolated snapshot.
     */
    Snapshot apply(DeltaSnapshot& delta, float interp = 1.0f);

    /**
     * @return the tick number of this snapshot
     */
    std::uint32_t tick() const;

    /**
     * Adds an object to the snapshot, takes the ownership of the object.
     */
    void add_object(std::shared_ptr<GameObject> object);

    /**
     * Deletes a game object from the snapshot from the game object id.
     * @param id, the id of the game object to remove
     * @return true if the object has been successfully removed, false
     * otherwise.
     */
    bool delete_object(std::uint32_t id);

    /**
     * Returns the game object pointer by id. Returns nullptr if no object has
     * been found.
     * @return pointer to object if found, nullptr if not found.
     */
    std::shared_ptr<GameObject> get_object(std::uint32_t id);

    std::shared_ptr<const GameObject> get_object(std::uint32_t id) const;

private:
    std::map<std::uint32_t, std::shared_ptr<GameObject>> _objects;

    std::uint32_t _tick;
};

/**
 * A delta snapshot contains all the state differences between two snapshots.
 * The difference being
 */
class DeltaSnapshot : std::enable_shared_from_this<DeltaSnapshot> {
public:
    DeltaSnapshot(std::uint32_t prev_tick, std::uint32_t next_tick);

    /**
     * Move constructor, as it is moved away to the heap in order to be used as
     * a pointer.
     */
    DeltaSnapshot(DeltaSnapshot&& delta_snapshot);

    /**
     * Builds the _delta_values map by comparing each object of the two
     * snapshots.
     */
    void evaluate(Snapshot const& prev_snap, Snapshot const& next_snap);

    /**
     * Returns the evaluated differences between two snapshot.
     * @return a diffmap_t containing the snapshots differences.
     */
    diffmap_t const& delta_values();

    /**
     * Returns the evaluated list of deleted objects in the newer snapshot.
     * @return a object_map_t containing the deleted objects.
     */
    object_map_t const& deleted_objects();

    /**
     * Returns the evaluated list of added objects in the newer snapshot.
     * @return a object_map_t containing the added objects.
     */
    object_map_t const& added_objects();

private:
    std::uint32_t _prev_tick = 0;
    std::uint32_t _next_tick = 0;

    /** The key is the game object id, the Gamevalue is a state change */
    diffmap_t _delta_values;

    object_map_t _deleted_objects;
    object_map_t _added_objects;
};

} // namespace core
