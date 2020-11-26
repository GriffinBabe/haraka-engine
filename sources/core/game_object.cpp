#include "core/game_object.hpp"

inline bool core::GameObject::is_networked()
{
    return _networked;
}

core::diffset_t core::GameObject::compare(const core::GameObject* other) const
{
    diffset_t diffset;
    diffset.reserve(_values.size());

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
        diffset.push_back(
            std::pair<std::string, core::value_t>(value_name, delta_value));
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
