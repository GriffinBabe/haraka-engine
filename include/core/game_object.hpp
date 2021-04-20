#pragma once
#include "events.hpp"
#include <cinttypes>
#include <memory>

namespace core {
class GameObject : std::enable_shared_from_this<GameObject>,
                   public Observable,
                   public Observer {
public:
    GameObject(std::uint32_t id);

    ~GameObject();

    virtual void on_turn_begin() = 0;

    virtual void on_turn_end() = 0;

    [[nodiscard]] inline std::uint32_t id() const;

private:
    std::uint32_t _id;
};
} // namespace core

std::uint32_t core::GameObject::id() const
{
    return _id;
}
