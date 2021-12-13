#include "core/world_old.hpp"
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

void core::World::react_event(core::Observable* observable, core::Event& event)
{
    switch (event.type) {
    case Event::OBJECT_DELETED:
        for (const auto& [id, obj] : _objects) {
            // TODO might be dangerous
            auto* obs_ptr = (Observer*) observable;
            obj->remove_observer(obs_ptr);
        }
        break;
    }
}
