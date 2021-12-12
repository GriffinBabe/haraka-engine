#pragma once
#include "core/exception.hpp"
#include "core/game_object.hpp"
#include "core/world.hpp"

namespace core {

class GameAction : std::enable_shared_from_this<GameAction> {
public:
    explicit GameAction(std::uint32_t id) : _id(id)
    {
    }

    virtual void act(World& world)
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
    virtual bool check_action(World& obj) = 0;

    virtual void perform_act(World& obj) = 0;

    std::uint32_t _id;

};

class CharacterAction : public GameAction {
public:
    explicit CharacterAction(std::uint32_t id) : core::GameAction(id)
    {
    }

private:
};
} // namespace core
