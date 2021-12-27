#include "core/snapshot.hpp"
#include "core/exception.hpp"
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
    if (interp > 1.0f || interp < 0.0f) {
        std::stringstream ss;
        ss << "Cannot interpolate snapshot with delta value: " << interp << ".";
        throw core::HarakaException(ss.str());
    }
    Snapshot next(*this);
    if (interp == 1.0f) {
        // add added objects
        for (const auto& obj : delta.added_objects()) {
            next.add_object(obj.second);
        }
        // remove removed objexts
        for (auto deleted_id : delta.deleted_objects()) {
            bool deleted = next.delete_object(deleted_id);
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
core::Snapshot
core::Snapshot::deserialize(core::serialization::Snapshot const& object)
{
    Snapshot snapshot(object.tick());
    auto const& object_dict = object.objects();
    for (auto it = object_dict.begin(); it != object_dict.end(); it++) {
        std::shared_ptr<GameObject> game_object =
            core::GameObject::deserialize(*it);
        snapshot.add_object(game_object);
    }
    return snapshot;
}
core::serialization::Snapshot core::Snapshot::serialize() const
{
    serialization::Snapshot snapshot;
    snapshot.set_tick(_tick);
    for (auto const& pair : _objects) {
        auto serialized_object = pair.second->serialize();
        // creates a pointer to an allocated space inside the buffer
        auto* serialized_buffer_ptr = snapshot.add_objects();
        // sets the element into the allocated space
        *serialized_buffer_ptr = serialized_object;
    }
    return snapshot;
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
            _deleted_objects.push_back(object_id);
        }
        else if (prev_object->checksum() != next_object->checksum()) {
            // object still exists so we keep it in delta vlaue if it has
            // changed (different checksum)
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

const core::id_list_t& core::DeltaSnapshot::deleted_objects()
{
    return _deleted_objects;
}
const core::object_map_t& core::DeltaSnapshot::added_objects()
{
    return _added_objects;
}

core::DeltaSnapshot core::DeltaSnapshot::deserialize(
    const core::serialization::DeltaSnapshot& object,
    core::Snapshot const& reference_snapshot)
{
    core::DeltaSnapshot delta_snapshot(object.prev_tick(), object.next_tick());

    auto const& object_values_dict = object.delta_objects();
    // iterates for each object in the diffmap
    for (auto const& pair : object_values_dict) {
        diffset_t value_differences;
        auto object_id = pair.first;
        // get the object reference, used to deserialize game values
        auto object_reference = reference_snapshot.get_object(object_id);
        // iterates for each object game value
        for (auto const& value_pair : pair.second.values()) {
            // gets the object reference value
            auto* object_reference_value =
                object_reference->get_value(value_pair.first);
            // deserialize from our object reference value
            auto deserialized_game_value =
                object_reference_value->deserialize(value_pair.second);
            // insert the deserialized game value in the diffset with the value
            // name.
            value_differences.insert(
                {value_pair.first, deserialized_game_value});
        }
        delta_snapshot._delta_values.insert({object_id, value_differences});
    }

    // copies deleted object ids
    std::copy(object.deleted_objects().begin(),
              object.deleted_objects().end(),
              std::back_inserter(delta_snapshot._deleted_objects));

    // adds added objects
    for (auto const& added_object : object.added_objects()) {
        std::shared_ptr<GameObject> deserialized_object =
            core::GameObject::deserialize(added_object);
        delta_snapshot._added_objects.insert(
            {deserialized_object->id(), deserialized_object});
    }

    return delta_snapshot;
}

core::serialization::DeltaSnapshot core::DeltaSnapshot::serialize() const
{
    serialization::DeltaSnapshot delta_snapshot;
    delta_snapshot.set_prev_tick(_prev_tick);
    delta_snapshot.set_next_tick(_next_tick);

    // serializes delta differences
    auto& delta_objects = *delta_snapshot.mutable_delta_objects();
    for (auto const& delta_value_pair : _delta_values) {
        serialization::ValueMap value_map;
        auto* serialized_values = value_map.mutable_values();
        for (auto const& values : delta_value_pair.second) {
            // add to serialized_values
            serialized_values->insert(
                {values.first, values.second->serialize()});
        }
        delta_objects[delta_value_pair.first] = value_map;
    }

    // serializes added objects
    for (auto const& object_pair : _added_objects) {
        auto* obj_ptr = delta_snapshot.add_added_objects();
        *obj_ptr = object_pair.second->serialize();
    }

    // serializes deleted objects
    for (auto object_id : _deleted_objects) {
        delta_snapshot.add_deleted_objects(object_id);
    }

    return delta_snapshot;
}
