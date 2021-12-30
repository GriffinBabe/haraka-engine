add_executable(
        tutorials_asio
        tutorials/boost_asio_core.cpp
)

target_link_libraries(tutorials_asio PUBLIC ${Boost_LIBRARIES} Threads::Threads)

add_executable(
        tutorials_asio_tcp
        tutorials/boost_asio_tcp.cpp
)

target_link_libraries(tutorials_asio_tcp PUBLIC ${Boost_LIBRARIES} Threads::Threads)

#add_executable(
#        tutorials_protobuf
#        tutorials/google_protobufs.cpp
#        tutorials/protobufs/connection.pb.cc
#)
#
#target_link_libraries(tutorials_protobuf PUBLIC ${Protobuf_LIBRARIES})

add_executable(
        test_server
        net/test_server.cpp
        net/protobufs/dummy_packets.pb.cc
)

target_link_libraries(test_server PUBLIC network GTest::Main ${Protobuf_LIBRARIES} Threads::Threads)


add_executable(
        test_types
        core/test_types.cpp
)

target_link_libraries(test_types PUBLIC core GTest::Main)

add_executable(
        test_snapshots
        core/test_snapshots.cpp
)

target_link_libraries(test_snapshots PUBLIC core GTest::Main)

add_executable(
        test_actions
        core/test_action.cpp
)

target_link_libraries(test_actions PUBLIC core GTest::Main)

add_executable(
        test_gameinstance
        core/test_gameinstance.cpp
)

target_link_libraries(test_gameinstance PUBLIC core GTest::Main)

add_executable(
        test_objects
        core/test_objects.cpp
)

target_link_libraries(test_objects PUBLIC core GTest::Main)

#add_executable(
#        test_server_client
#        server_client/test_server_client.cpp
#)
#
#target_link_libraries(test_server_client PUBLIC core server client)

gtest_discover_tests(test_server)
gtest_discover_tests(test_types)
gtest_discover_tests(test_snapshots)
gtest_discover_tests(test_actions)
gtest_discover_tests(test_gameinstance)
gtest_discover_tests(test_objects)
#gtest_discover_tests(test_server_client)
