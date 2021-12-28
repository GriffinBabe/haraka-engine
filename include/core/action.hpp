#pragma once
#include "core/exception.hpp"
#include "core/game_object.hpp"
#include "core/serialization/action_serialization.pb.h"
#include "core/snapshot.hpp"

namespace core {

class GameAction : std::enable_shared_from_this<GameAction> {
public:
    typedef std::unique_ptr<GameAction> (*base_creator_fn)();
    typedef std::unordered_map<std::string, base_creator_fn> registry_map;

    explicit GameAction(std::uint32_t id) : _id(id)
    {
    }

    explicit GameAction() : _id(0)
    {
    }

    virtual void act(Snapshot& world)
    {
        bool activable = check_action(world);
        if (!activable) {
            throw core::ImpossibleActionException("Action cannot be activated",
                                                  *this);
        }
        perform_act(world);
    }

    [[nodiscard]] std::uint32_t id()
    {
        return _id;
    }

    static std::unique_ptr<GameAction> instantiate(std::string const& type);

    static registry_map& registry();

    serialization::Action serialize() const;

    static std::unique_ptr<GameAction>
    deserialize(serialization::Action const& buffer);

protected:
    virtual void add_values() = 0;

    virtual bool check_action(const Snapshot& obj) = 0;

    virtual void perform_act(Snapshot& obj) = 0;

    virtual std::string type_name() const = 0;

    std::uint32_t _id;

    std::map<std::string, GameValue*> _values;
};

/**
 * Factory creator for GameAction
 */
struct ActionRegistrar {
    ActionRegistrar(std::string name, GameAction::base_creator_fn func);
};

/**
 * Status to report if an action failed.
 */
struct ActionStatus {
    std::uint32_t action_id;
    bool success;
    std::string message;

    serialization::ActionStatus serialize() const;

    static ActionStatus deserialize(serialization::ActionStatus const& buffer);
};

serialization::ActionStatusList
serialize_action_status_list(std::vector<ActionStatus> const& status_list);

std::vector<ActionStatus> deserialize_action_status_list(serialization::ActionStatusList const& status_list);

} // namespace core
