#include "core/gameinstance.hpp"
#include "core/exception.hpp"

core::GameInstance::GameInstance(core::World& world)
    : _world(world)
{
}

bool core::GameInstance::play_action(std::shared_ptr<core::GameAction> action)
{
    try {
        action->act(_world);
        return true;
    }
    catch (core::UnknownIDException const& exc) {
        // TODO add log
        return false;
    }
    catch (core::ImpossibleActionException const& exc) {
        // TODO add log
        return false;
    }
}

void core::GameInstance::add_player(core::Player const& player)
{
    _players.push_back(player);
}
