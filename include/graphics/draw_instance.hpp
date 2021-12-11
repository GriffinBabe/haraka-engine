#pragma once

#include "core/game_object_old.hpp"
namespace gfx {

class DrawInstance : public core::Observer {
public:

    DrawInstance(std::shared_ptr<core::GameObject>& object);

    ~DrawInstance();

private:
    std::shared_ptr<core::GameObject> _object;
};

} // namespace gfx
