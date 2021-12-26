#pragma once
#include "core/events.hpp"
#include "core/serialization/object_serialization.pb.h"
#include "core/types.hpp"
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace core {

/**
 * Set of differences between two similar objects. The std::string is the name
 * while the std::shared_ptr<GameValue> (core::value_t) is the delta value.
 */
typedef std::map<std::string, core::value_t> diffset_t;

/**
 * Abstract class that consist of all entities and attributes.
 */
class GameObject : std::enable_shared_from_this<GameObject>,
                   public Observer,
                   public Observable {
public:
    typedef std::unique_ptr<GameObject> (*base_creator_fn)();
    typedef std::unordered_map<std::string, base_creator_fn> registry_map;


    GameObject(std::uint32_t id);

    /**
     * Default constructor, sets id to 0.
     */
    GameObject();

    /**
     * Copy constructor.
     * @param obj, the object to deep copy.
     */
    GameObject(GameObject const& obj) = delete;

    virtual ~GameObject() = default;

    /**
     * Returns a copy of the game object with interpolated values.
     * @param differences, the difference map on the gameobject's values.
     * @param interp, the interpolation time. The maximum being 1.0 and the
     * minimum 0.0
     * @return a object with interpolated game state values.
     */
    std::unique_ptr<GameObject> interpolate(diffset_t const& differences,
                                            float interp);

    /**
     * Updates physics, time dependant mechanics.
     * @param delta_time
     */
    virtual void update(float delta_time) = 0;

    /**
     * @return an unique pointer to a deepcopy of this game object.
     */
    virtual std::unique_ptr<GameObject> clone() = 0;

    /**
     * Gets the object's type name. (Same as used in the Factory)
     */
    virtual std::string type_name() = 0;

    /**
     * Computes the object checksum by performing exclusive or operations on
     * each object game value checksum. This can be used to check wherever the
     * object has changed and needs to be included in a delta snapshot.
     *
     * @return a std::uint32_t checksum for the object.
     */
    std::uint32_t checksum() const;

    /**
     * Iterates over all the mapped GameValues and get the delta values by
     * comparing with the other GameObject. Returns a list of those delta values
     * with the corresponding variable name.
     *
     * @param other, the other object to compare with.
     * @return a list of delta values.
     */
    diffset_t compare(GameObject const* other) const;

    /**
     * Returns an object's game value.
     * @param name, the name of the value
     * @return a game value.
     */
    [[nodiscard]] GameValue const* get_value(std::string const& name) const;

    /**
     * @return the object's id in the game
     */
    [[nodiscard]] std::uint32_t id() const;

    /**
     * Serializes the object, returning a serialization object defined by
     * protocol buffers. This object contains the object id, object type id
     * (from factory) and all the GameValues serialized.
     * @return a serialization object.
     */
    serialization::GameObject serialize();

    /**
     * Deserializes an object, reading first the object type id (from factory) and
     * sets the object id and its game values.
     */
    static std::unique_ptr<GameObject> deserialize(serialization::GameObject object);

    /**
     * Factory registry map, contains all the derived game objects types from
     * their name.
     */
     static registry_map& registry();

     /**
      * Calls factory constructor and creates an object from its name.
      *
      * Should not be used outside the class
      * TODO: Then maybe move to private members?
      * @param type, the name of the GameObject
      * @return a shared_ptr to the create GameObject
      */
     static std::unique_ptr<GameObject> instantiate(std::string const& type);

protected:
    /**
     * Adds a pointer to the _values map for each member value.
     * ALWAYS CALL IT IN THE SUBCLASS CONSTRUCTORS
     */
    virtual void add_values() = 0;

    std::map<std::string, GameValue*> _values;

    std::uint32_t _id = 0;
};


struct Registrar
{
    Registrar(std::string name, GameObject::base_creator_fn func);
};

} // namespace core
