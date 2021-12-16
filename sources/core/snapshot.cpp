#include "core/snapshot.hpp"
#include <cassert>
#include <sstream>

core::Snapshot::Snapshot(std::uint32_t tick) : _tick(tick)
{
}

core::Snapshot::Snapshot(const core::Snapshot& other) : _tick(other._tick + 1)
{
    for (auto& obj : other._objects) {
        this->_objects[obj.first] = obj.second->clone();
    }
}

std::uint32_t core::Snapshot::tick() const
{
    return _tick;
}

void core::Snapshot::add_object(std::shared_ptr<GameObject> object)
{
    auto id = object->id();
    _objects[id] = object;
}

std::shared_ptr<core::GameObject> core::Snapshot::get_object(std::uint32_t id)
{
    auto it = _objects.find(id);
    if (it != _objects.end()) {
        return it->second;
    }
    return nullptr;
}
std::shared_ptr<const core::GameObject>
core::Snapshot::get_object(std::uint32_t id) const
{
    auto it = _objects.find(id);
    if (it != _objects.end()) {
        return it->second;
    }
    return nullptr;
}

void core::Snapshot::update(float delta_time)
{
    auto it = _objects.begin();
    while (it != _objects.end()) {
        it->second->update(delta_time);
        it++;
    }
}

core::Snapshot core::Snapshot::apply(core::DeltaSnapshot& delta, float interp)
{
    Snapshot next(*this);
    if (interp == 1.0f) {
        // add added objects
        for (const auto& obj : delta.added_objects()) {
            next.add_object(obj.second);
        }
        // remove removed objexts
        for (const auto& obj : delta.deleted_objects()) {
            bool deleted = next.delete_object(obj.first);
#ifndef NDEBUG
            assert(deleted);
#endif
        }
    }
    for (const auto& obj : delta.delta_values()) {
        auto object_id = obj.first;
        auto object = get_object(object_id);
#ifndef NDEBUG
        assert(object != nullptr);
#endif
        auto new_object = object->interpolate(obj.second, interp);
        next._objects[object_id] = std::move(new_object);
    }
    return next;
}

bool core::Snapshot::delete_object(std::uint32_t id)
{
    auto it = _objects.find(id);
    if (it != _objects.end()) {
        _objects.erase(it);
        return true;
    }
    return false;
}

core::DeltaSnapshot::DeltaSnapshot(std::uint32_t prev_snap,
                                   std::uint32_t next_snap)
    : _prev_tick(prev_snap), _next_tick(next_snap)
{
}

core::DeltaSnapshot::DeltaSnapshot(core::DeltaSnapshot&& delta_snapshot)
    : _prev_tick(delta_snapshot._prev_tick),
      _next_tick(delta_snapshot._next_tick),
      _delta_values(std::move(delta_snapshot._delta_values)),
      _deleted_objects(std::move(delta_snapshot._deleted_objects)),
      _added_objects(std::move(delta_snapshot._added_objects))
{
    delta_snapshot._prev_tick = 0;
    delta_snapshot._next_tick = 0;
}

void core::DeltaSnapshot::evaluate(const core::Snapshot& prev_snap,
                                   const core::Snapshot& next_snap)
{
    // Checks for deleted objects AND delta values between the two snapshots
    auto it = prev_snap._objects.begin();
    while (it != prev_snap._objects.end()) {
        std::uint32_t object_id = it->first;
        auto prev_object = prev_snap.get_object(object_id);
        auto next_object = next_snap.get_object(object_id);
#ifndef NDEBUG
        assert(prev_object != nullptr);
#endif
        if (next_object == nullptr) {
            // object has been deleted in the new snapshot so we added it in the
            // deleted objects
            _deleted_objects[object_id] =
                std::const_pointer_cast<GameObject>(prev_object);
        }
        else {
            // object still exists so we keep it
            auto differences = prev_object->compare(next_object.get());
            _delta_values[prev_object->id()] = differences;
        }
        it++;
    }
    // Checks the added objects between the two snapshots
    it = next_snap._objects.begin();
    while (it != next_snap._objects.end()) {
        std::uint32_t object_id = it->first;
        auto next_object = next_snap.get_object(object_id);
        auto prev_object = prev_snap.get_object(object_id);
#ifndef NDEBUG
        assert(next_object != nullptr);
#endif
        if (prev_object == nullptr) {
            _added_objects[object_id] =
                std::const_pointer_cast<GameObject>(next_object);
        }
        it++;
    }
}

const core::diffmap_t& core::DeltaSnapshot::delta_values()
{
    return _delta_values;
}
const core::object_map_t& core::DeltaSnapshot::deleted_objects()
{
    return _deleted_objects;
}
const core::object_map_t& core::DeltaSnapshot::added_objects()
{
    return _added_objects;
}
