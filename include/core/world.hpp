#pragma once
#include "core/game_object.hpp"
#include <map>

namespace core {

/**
 * World contains all GameObject such as characters, environment, projectiles
 */
class World {
public:

    World();

    /**
     * Get game object from id.
     * @param id, the gameobject's id
     * @return shared ptr to gameobject.
     * @throws UnknownIDException if the id is unknown
     */
    std::shared_ptr<core::GameObject> get_gameobject(std::uint32_t id);

    /**
     * Adds a game object to the gameobject map.
     * @param obj
     */
    void add_gameobject(std::shared_ptr<GameObject> obj);

private:

    /**
     * Map containing all gameobjects.
     */
    std::map<std::uint32_t, std::shared_ptr<core::GameObject>> _objects;
};

} // namespace core