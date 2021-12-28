#include "graphics/draw_instance.hpp"

gfx::DrawInstance::DrawInstance(std::shared_ptr<core::GameObject>& object)
    : _object(object)
{
    _object->add_observer(this);
}

gfx::DrawInstance::~DrawInstance()
{
    _object->remove_observer(this);
}
