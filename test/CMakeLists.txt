add_executable(
        tutorials_asio
        tutorials/boost_asio_core.cpp
)

target_link_libraries(tutorials_asio PUBLIC ${Boost_LIBRARIES})

add_executable(
        tutorials_asio_tcp
        tutorials/boost_asio_tcp.cpp
)

target_link_libraries(tutorials_asio PUBLIC ${Boost_LIBRARIES})

add_executable(
        tutorials_protobuf
        tutorials/google_protobufs.cpp
        tutorials/protobufs/connection.pb.cc
)

target_link_libraries(tutorials_protobuf PUBLIC ${Protobuf_LIBRARIES})

add_executable(
        test_server
        net/test_server.cpp
        net/protobufs/dummy_packets.pb.cc
)

target_link_libraries(test_server PUBLIC network GTest::Main ${Protobuf_LIBRARIES})

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

gtest_discover_tests(test_server)
