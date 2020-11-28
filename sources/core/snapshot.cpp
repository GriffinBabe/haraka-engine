#include "core/snapshot.hpp"

core::Snapshot::Snapshot(std::uint32_t tick)
    : _tick(tick)
{
}

core::Snapshot::Snapshot(const core::Snapshot& other)
    : _tick(other._tick + 1)
{
    for (auto& obj : other._objects) {
        this->_objects[obj.first] = std::unique_ptr<GameObject>(obj.second->clone());
    }
}

std::uint32_t core::Snapshot::tick() const
{
    return _tick;
}

void core::Snapshot::add_object(std::uint32_t id,
                                std::unique_ptr<GameObject>& object)
{
    _objects[id] = std::move(object);
}

core::GameObject* core::Snapshot::get_object(std::uint32_t id)
{
    auto it = _objects.find(id);
    if (it != _objects.end()) {
        return it->second.get();
    }
    return nullptr;
}
const core::GameObject* core::Snapshot::get_object(std::uint32_t id) const
{
    auto it = _objects.find(id);
    if (it != _objects.end()) {
        return it->second.get();
    }
    return nullptr;
}

core::DeltaSnapshot::DeltaSnapshot(std::uint32_t prev_snap,
                                   std::uint32_t next_snap)
    : _prev_tick(prev_snap), _next_tick(next_snap)
{
}

void core::DeltaSnapshot::evaluate(const core::Snapshot& prev_snap,
                                   const core::Snapshot& next_snap)
{
    // Checks for deleted objects AND delta values between the two snapshots
    auto it = prev_snap._objects.begin();
    while (it != prev_snap._objects.end()) {
        std::uint32_t object_id = it->first;
        GameObject const* prev_object = prev_snap.get_object(object_id);
        GameObject const* next_object = next_snap.get_object(object_id);
#ifndef NDEBUG
       assert(prev_object != nullptr);
#endif
       if (next_object == nullptr) {
           // object has been deleted in the new snapshot so we added it in the
           // deleted objects
           _deleted_objects[object_id] = prev_object;
       }
       else {
            // object still exists so we keep it
            auto differences = prev_object->compare(next_object);
            for (auto& pair : differences) {
                _delta_values[object_id].push_back(pair);
            }
       }
       it++;
    }
    // Checks the added objects between the two snapshots
    it = next_snap._objects.begin();
    while (it != next_snap._objects.end()) {
        std::uint32_t  object_id = it->first;
        GameObject const* next_object = next_snap.get_object(object_id);
        GameObject const* prev_object = prev_snap.get_object(object_id);
#ifndef NDEBUG
        assert(next_object != nullptr);
#endif
        if (prev_object == nullptr) {
            _added_objects[object_id] = next_object;
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
