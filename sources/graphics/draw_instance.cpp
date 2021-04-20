#include "graphics/draw_instance.hpp"

gfx::DrawInstance::DrawInstance(std::shared_ptr<core::GameObject>& object)
    : _object(object)
{
}
