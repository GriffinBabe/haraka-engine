#pragma once
#include "core/events.hpp"
#include "core/types.hpp"
#include <map>
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
    GameObject(std::uint32_t id);

    /**
     * Copy constructor.
     * @param obj, the object to deep copy.
     */
    GameObject(GameObject const& obj) = delete;

    virtual ~GameObject() = default;

    /**
     * Returns a copy of the game object with interpolated values.
     * @param differences, the difference map on the gameobject's values.
     * @param interp, the interpolation time. The maximum being 1.0 and the minimum 0.0
     * @return a object with interpolated game state values.
     */
    std::unique_ptr<GameObject> interpolate(diffset_t const& differences, float interp);

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
     * Iterates over all the mapped GameValues and get the delta values by
     * comparing with the other GameObject. Returns a list of those delta values
     * with the corresponding variable name.
     *
     * @param other, the other object to compare with.
     * @return a list of delta values.
     */
    diffset_t compare(GameObject const* other) const;

    [[nodiscard]] bool is_networked();

    [[nodiscard]] GameValue const* get_value(std::string const& name) const;

    [[nodiscard]] std::uint32_t id() const;

protected:
    /**
     * Adds a pointer to the _values map for each member value.
     * ALWAYS CALL IT IN THE SUBCLASS CONSTRUCTORS
     */
    virtual void add_values() = 0;

    bool _networked = false;

    std::map<std::string, GameValue*> _values;

    const std::uint32_t _id = 0;
};

} // namespace core
