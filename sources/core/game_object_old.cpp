#include "core/game_object_old.hpp"

core::GameObject::GameObject(std::uint32_t id)
    : _id(id)
{
}

core::GameObject::~GameObject()
{
    Event event(Event::OBJECT_DELETED);
    notify(event);
}