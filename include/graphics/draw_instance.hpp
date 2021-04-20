#pragma once

#include "core/game_object.hpp"
namespace gfx {

class DrawInstance {
public:

    DrawInstance(std::shared_ptr<core::GameObject>& object);

private:
    std::shared_ptr<core::GameObject> _object;
};

} // namespace gfx
