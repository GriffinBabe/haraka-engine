#include "core/gameinstance.hpp"
#include "core/world_old.hpp"
#include "graphics/window.hpp"

int main(int argc, char** argv)
{
    core::World world;
    core::GameInstance instance(world);

    core::Team team_blue(core::Team::BLUE);
    core::Team team_red(core::Team::RED);

    core::Player player_1(0, team_blue);
    core::Player player_2(1, team_red);

    instance.add_player(player_1);
    instance.add_player(player_2);

    gfx::Window window;
    window.init();

    while (true) {
        window.clear();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                window.destroy();
                return 0;
                break;
            default:
                break;
            }
        }

        window.draw();
    }

}