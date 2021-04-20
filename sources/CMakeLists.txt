add_library(core SHARED
        core/game_object.cpp
        core/gameinstance.cpp
        core/world.cpp
        core/exception.cpp
        core/action.cpp
        core/teams.cpp
        core/player.cpp
        core/types.cpp
        core/events.cpp
        )

add_library(
        network SHARED
        net/server.cpp
        net/tsdeque.cpp
        net/session.cpp
        net/packet.cpp
        net/client.cpp
)

target_link_libraries(network PUBLIC ${Boost_LIBRARIES} Threads::Threads)

add_library(
        graphics SHARED
        graphics/window.cpp
        graphics/draw_instance.cpp
)

target_link_libraries(graphics PUBLIC ${SDL2_LIBRARIES} core)

add_executable(
        game
        game/game.cpp
)

target_link_libraries(game PUBLIC core network graphics)
