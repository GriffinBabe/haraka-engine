add_library(core SHARED
        core/types.cpp
        core/snapshot.cpp
        core/game_object.cpp
)

add_library(
        network SHARED
        net/server.cpp
        net/tsdeque.cpp
        net/session.cpp
        net/packet.cpp
)

target_link_libraries(network PUBLIC ${Boost_LIBRARIES})
