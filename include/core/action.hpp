#pragma once
#include "core/exception.hpp"
#include "core/game_object.hpp"
#include "core/snapshot.hpp"

namespace core {

class GameAction : std::enable_shared_from_this<GameAction> {
public:
    explicit GameAction(std::uint32_t id) : _id(id)
    {
    }

    virtual void act(Snapshot& world)
    {
        bool activable = check_action(world);
        if (!activable) {
            throw core::ImpossibleActionException(
                "Action cannot be activated", *this);
        }
        perform_act(world);
    }

    [[nodiscard]] std::uint32_t id()
    {
        return _id;
    }

protected:
    virtual bool check_action(const Snapshot& obj) = 0;

    virtual void perform_act(Snapshot& obj) = 0;

    std::uint32_t _id;

};

/**
 * Status to report if an action failed.
 */
struct ActionStatus {
    std::shared_ptr<GameAction> action;
    bool success;
    std::string message;
};

} // namespace core
