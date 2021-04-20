#pragma once
#include "core/game_object_snapshot.hpp"
#include <map>

namespace core {

/**
 * Contains all the differences between all the objects of two snapshots.
 *
 * std::uint32_t is the id of the object in the snapshot.
 * std::string is the name of the variables
 * core::value_t is the delta difference.
 */
typedef std::map<std::uint32_t,
                 std::vector<std::pair<std::string, core::value_t>>>
    diffmap_t;

typedef std::map<std::uint32_t, GameObject const*> object_map_t;

/**
 * A snapshot contains all game state information at a giving time.
 */
class Snapshot {
    friend class DeltaSnapshot;

public:
    Snapshot(std::uint32_t tick);

    /**
     * Copy constructor. Will copy all the data in the memory. The tick is by
     * default incremented by one.
     * @param other
     */
    Snapshot(Snapshot const& other);

    std::uint32_t tick() const;

    /**
     * Adds an object to the snapshot, takes the ownership of the object.
     */
    void add_object(std::uint32_t id, std::unique_ptr<GameObject>& object);

    /**
     * Returns the game object pointer by id. Returns nullptr if no object has
     * been found.
     * @return pointer to object if found, nullptr if not found.
     */
    GameObject* get_object(std::uint32_t id);

    GameObject const* get_object(std::uint32_t id) const;

private:
    std::map<std::uint32_t, std::unique_ptr<GameObject>> _objects;

    std::uint32_t _tick;
};

/**
 * A delta snapshot contains all the state differences between two snapshots.
 * The difference being
 */
class DeltaSnapshot {
public:
    DeltaSnapshot(std::uint32_t prev_tick, std::uint32_t next_tick);

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
