#include "core/world.hpp"
#include "core/exception.hpp"

core::World::World()
{
}

void core::World::add_gameobject(std::shared_ptr<GameObject> obj)
{
    _objects.insert(
        {obj->id(), obj});
}


std::shared_ptr<core::GameObject> core::World::get_gameobject(std::uint32_t id)
{
    try {
        auto ptr = _objects.at(id);
        return ptr;
    }
    catch (std::out_of_range const& exc) {
        throw core::UnknownIDException("Unknown object id.", id);
    }
}
