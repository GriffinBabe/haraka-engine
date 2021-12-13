add_library(
        util SHARED
        util/tsdeque.cpp
)

add_library(core SHARED
        core/game_object.cpp
        core/gameinstance.cpp
        core/exception.cpp
        core/action.cpp
        core/teams.cpp
        core/player.cpp
        core/types.cpp
        core/snapshot.cpp
        core/events.cpp
        )

target_link_libraries(core PUBLIC util)

add_library(
        network SHARED
        net/server.cpp
        net/session.cpp
        net/packet.cpp
        net/client.cpp
)

target_link_libraries(network PUBLIC util ${Boost_LIBRARIES} Threads::Threads)

add_library(
        graphics SHARED
        graphics/window.cpp
        graphics/draw_instance.cpp
)

target_link_libraries(graphics PUBLIC ${SDL2_LIBRARIES} core)
