cmake_minimum_required(VERSION 3.10)
project(itsamonster)

set(CMAKE_CXX_STANDARD 20)

add_executable(itsamonster src/itsamonster.cpp)

target_include_directories(itsamonster PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_sources(itsamonster PRIVATE
    src/Action.cpp
    src/Action.hpp
    src/Match.cpp
    src/Match.hpp
    src/Monster.cpp
    src/Monster.hpp
    src/Types.hpp
    src/YoungGoldDragon.hpp
    src/Yochlol.hpp
)

