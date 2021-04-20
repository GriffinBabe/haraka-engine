#include "graphics/window.hpp"
#include <iostream>

gfx::Window::Window(int width, int height) : _width(width), _height(height)
{
}

bool gfx::Window::init()
{
    int render_flags, window_flags;

    render_flags = SDL_RENDERER_ACCELERATED;

    window_flags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        // TODO add log
        std::cout << "Problem initializing video" << std::endl;
        return false;
    }

    _window = SDL_CreateWindow("Haraka game engine",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_CENTERED,
                               _width,
                               _height,
                               window_flags);

    if (!_window) {
        // TODO add log
        std::cout << "Problem initializing window." << std::endl;
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    _renderer = SDL_CreateRenderer(_window, -1, render_flags);

    if (!_renderer) {
        // TODO add log
        std::cout << "Problem initializing renderer." << std::endl;
        return false;
    }

    return true;
}

void gfx::Window::clear()
{
    SDL_SetRenderDrawColor(_renderer, 96, 128, 255, 255);
    SDL_RenderClear(_renderer);
}

void gfx::Window::draw()
{
    SDL_RenderPresent(_renderer);
}
