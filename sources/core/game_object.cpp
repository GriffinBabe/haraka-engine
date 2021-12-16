#include "core/game_object.hpp"
#include <cassert>

core::GameObject::GameObject(std::uint32_t id) : _id(id)
{
}

inline bool core::GameObject::is_networked()
{
    return _networked;
}

std::unique_ptr<core::GameObject>
core::GameObject::interpolate(diffset_t const& differences, float interp)
{
    auto new_object = clone();
    for (auto const& value : differences) {
        auto object_value = new_object->_values[value.first];
        auto interpolated_value = object_value->interp(value.second.get(), interp);
        object_value->change(interpolated_value);
    }
    return new_object;
}

core::diffset_t core::GameObject::compare(const core::GameObject* other) const
{
    diffset_t diffset;

    auto it = _values.begin();
    while (it != _values.end()) {
        auto value_name = it->first;
        auto* value = it->second;
        auto* other_value = other->_values.at(value_name);
#ifndef NDEBUG
        // value should normally never be nullptr
        // other_value might be null ptr if two compared object are not the
        // same.
        assert(other_value != nullptr);
#endif
        auto delta_value = value->get_delta(other_value);
        diffset.insert({value_name, delta_value});
        it++;
    }

    return diffset;
}

const core::GameValue*
core::GameObject::get_value(std::string const& name) const
{
    GameValue const* value = _values.at(name);
#ifndef NDEBUG
    assert(value != nullptr);
#endif
    return value;
}

std::uint32_t core::GameObject::id() const
{
    return _id;
}
