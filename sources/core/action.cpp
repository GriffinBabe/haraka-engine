#include "core/action.hpp"
#include "core/exception.hpp"

core::ActionRegistrar::ActionRegistrar(std::string name,
                                       GameAction::base_creator_fn func)
{
    GameAction::registry()[name] = func;
    std::cout << "Registering action: '" << name << "'\n";
}

core::GameAction::registry_map& core::GameAction::registry()
{
    static registry_map map;
    return map;
}

core::serialization::Action core::GameAction::serialize() const
{
    serialization::Action action;
    action.set_id(_id);
    action.set_type_name(type_name());

    auto& value_map = *action.mutable_values();
    for (auto const& value_pair : _values) {
        value_map[value_pair.first] = value_pair.second->serialize();
    }

    return action;
}

std::unique_ptr<core::GameAction>
core::GameAction::deserialize(const core::serialization::Action& buffer)
{
    auto new_action = core::GameAction::instantiate(buffer.type_name());

    new_action->_id = buffer.id();
    auto const& value_dict = buffer.values();

    new_action->add_values();

    for (auto const& value_pair : value_dict) {
        auto old_value_ptr = new_action->_values[value_pair.first];
        auto deserialized_value = old_value_ptr->deserialize(value_pair.second);
        old_value_ptr->change(deserialized_value);
    }

    return new_action;
}

std::unique_ptr<core::GameAction>
core::GameAction::instantiate(const std::string& type)
{
    auto it = GameAction::registry().find(type);
    if (it == GameAction::registry().end()) {
        throw UnknownClassTypeException("Unknown class type.", type);
    }
    return (it->second)();
}

core::serialization::ActionStatus core::ActionStatus::serialize() const
{
    serialization::ActionStatus buffer;

    buffer.set_id(action_id);
    buffer.set_success(success);
    buffer.set_message(message);

    return buffer;
}

core::ActionStatus
core::ActionStatus::deserialize(serialization::ActionStatus const& buffer)
{
    ActionStatus status;

    status.action_id = buffer.id();
    status.message = buffer.message();
    status.success = buffer.success();

    return status;
}

core::serialization::ActionStatusList
core::serialize_action_status_list(const std::vector<ActionStatus>& status_list)
{
    core::serialization::ActionStatusList list;
    for (auto const& elem : status_list) {
        auto* serialized_status = list.add_status();
        *serialized_status = elem.serialize();
    }
    return list;
}

std::vector<core::ActionStatus> core::deserialize_action_status_list(
    core::serialization::ActionStatusList const& status_list)
{
    std::vector<ActionStatus> deserialized_status;
    auto const& serialized_status_list = status_list.status();
    for (auto const& serialized_status : serialized_status_list) {
        deserialized_status.push_back(
            core::ActionStatus::deserialize(serialized_status));
    }

    return deserialized_status;
}
