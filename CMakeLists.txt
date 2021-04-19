cmake_minimum_required(VERSION 3.16)
project(haraka_engine)

# Using C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED  True)

# Enable threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# For windows MSVC runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

find_package(GTest)
if (${GTEST_FOUND})
else()
    message(FATAL_ERROR Couldn't find GTest)
endif()

find_package(Boost)
if(${Boost_FOUND})
else()
    message(FATAL_ERROR "Couldn't find boost")
endif()

find_package(Protobuf)
if (${Protobuf_FOUND})
else()
    message(FATAL_ERROR "Couldn't find protobuf")
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Protobuf_INCLUDE_DIRS})
message(${Protobuf_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(include)

add_subdirectory(sources)
add_subdirectory(test)
