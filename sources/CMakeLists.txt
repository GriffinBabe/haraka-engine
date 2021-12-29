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
        core/serialization/object_serialization.pb.cc
        core/serialization/action_serialization.pb.cc
        )

target_link_libraries(core PUBLIC util ${Protobuf_LIBRARIES})

# Protobuf files for GameObject serialization
# PROTOBUF_GENERATE_CPP(${CMAKE_CURRENT_SOURCE_DIR}/core/serialization ${CMAKE_SOURCE_DIR}/include/core/serialization/ ${CMAKE_CURRENT_SOURCE_DIR}/core/serialization/object_serialization.proto)

add_library(
        network SHARED
        net/server.cpp
        net/session.cpp
        net/packet.cpp
        net/client.cpp
)

target_link_libraries(network PUBLIC util ${Boost_LIBRARIES} Threads::Threads)

add_library(
        server_lib SHARED
        server/serialization/server_serialization.pb.cc
        server/packet_types.cpp
        server/session_info.cpp
        server/server_controller.cpp
)

target_link_libraries(server_lib PUBLIC util core network)

add_library(
        client_lib SHARED
        client/client_controller.cpp
)

target_link_libraries(client_lib PUBLIC util core network server_lib)

add_library(
        graphics SHARED
        graphics/window.cpp
        graphics/draw_instance.cpp
)

target_link_libraries(graphics PUBLIC ${SDL2_LIBRARIES} core)
