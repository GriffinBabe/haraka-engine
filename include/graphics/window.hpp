#pragma once
#include <SDL.h>

namespace gfx
{

const int base_width = 1280;
const int base_height = 720;

class Window
{
public:

    Window(int width=base_width, int height=base_height);

    bool init();

    void clear();

    void draw();

private:
    SDL_Renderer* _renderer = nullptr;
    SDL_Window* _window = nullptr;

    int _width = 0;
    int _height = 0;
};

}