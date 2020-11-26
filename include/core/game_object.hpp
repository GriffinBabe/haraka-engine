#pragma once
#include "core/types.hpp"
#include <map>
#include <string>
#include <memory>
#include <vector>

namespace core {

/**
 * Set of differences between two similar objects. The std::string is the name
 * while the std::shared_ptr<GameValue> (core::value_t) is the delta value.
 */
typedef std::vector<std::pair<std::string, core::value_t>> diffset_t;

/**
 * Abstract class that consist of all entities and attributes.
 */
class GameObject {
public:
    GameObject() = default;

    virtual ~GameObject() = default;

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

protected:

    /**
     * Adds a pointer to the _values map for each member value.
     * ALWAYS CALL IT IN THE SUBCLASS CONSTRUCTORS
     */
    virtual void add_values() = 0;

    bool _networked = false;

    std::map<std::string, GameValue*> _values;
};

} // namespace core
