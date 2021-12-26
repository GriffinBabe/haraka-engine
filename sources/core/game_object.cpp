#include "core/game_object.hpp"
#include "core/exception.hpp"
#include <cassert>

core::GameObject::GameObject(std::uint32_t id) : _id(id)
{
}

core::GameObject::GameObject() : _id(0)
{
}

std::unique_ptr<core::GameObject>
core::GameObject::interpolate(diffset_t const& differences, float interp)
{
    auto new_object = clone();
    for (auto const& value : differences) {
        auto object_value = new_object->_values[value.first];
        auto interpolated_value =
            object_value->interp(value.second.get(), interp);
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

std::uint32_t core::GameObject::checksum() const
{
    boost::crc_32_type sum;
    for (const auto& it : _values) {
        std::uint32_t object_checksum = it.second->checksum();
        sum.process_bytes(&object_checksum, sizeof(std::uint32_t));
    }
    return sum.checksum();
}

core::serialization::GameObject core::GameObject::serialize()
{
    core::serialization::GameObject serialized;
    serialized.set_id(_id);

    serialized.set_type_name(type_name());

    auto& serialized_map = *serialized.mutable_values();
    for (const auto& value_pair : _values) {
        serialized_map[value_pair.first] = value_pair.second->serialize();
    }

    return serialized;
}

std::unique_ptr<core::GameObject>
core::GameObject::deserialize(core::serialization::GameObject object)
{
    auto new_object = core::GameObject::instantiate(object.type_name());

    new_object->_id = object.id();
    auto const& values_dict = *object.mutable_values();

    new_object->add_values();

    for (auto it = values_dict.begin(); it != values_dict.end(); it++) {
        auto value = new_object->_values[it->first]->deserialize(it->second);
        auto old_value_ptr = new_object->_values[it->first];
        old_value_ptr->change(value);
    }

    return new_object;
}

core::GameObject::registry_map& core::GameObject::registry()
{
    static registry_map map;
    return map;
}

std::unique_ptr<core::GameObject>
core::GameObject::instantiate(const std::string& type)
{
    auto it = GameObject::registry().find(type);
    if (it == GameObject::registry().end()) {
        throw UnknownClassTypeException("Unknown class type.", type);
    }
    return (it->second)();
}

core::Registrar::Registrar(std::string name,
                           core::GameObject::base_creator_fn func)
{
    GameObject::registry()[name] = func;
    std::cout << "Registering class: '" << name << "'\n";
}
