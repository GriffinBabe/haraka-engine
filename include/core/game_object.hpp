#pragma once
#include <cinttypes>
#include <memory>

namespace core
{
class GameObject : std::enable_shared_from_this<GameObject>
{
public:
    GameObject(std::uint32_t id);

    virtual ~GameObject() = default;

    virtual void on_turn_begin() = 0;

    virtual void on_turn_end() = 0;

    [[nodiscard]] inline std::uint32_t id() const;

private:
    std::uint32_t _id;
};
}

std::uint32_t core::GameObject::id() const
{
    return _id;
}
