cmake_minimum_required(VERSION 3.10)

project(itsamonster)

# Optimize for speed over size in Release mode
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ot /Ob2 /DNDEBUG")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Explicit source list
set(ITSAMONSTER_SOURCES
    src/itsamonster.cpp
    src/Action.cpp
    src/Action.hpp
    src/Battlefield.cpp
    src/Battlefield.hpp
    src/Match.cpp
    src/Match.hpp
    src/Monster.cpp
    src/Monster.hpp
    src/Types.hpp
    src/actions/RechargeAction.cpp
    src/actions/RechargeAction.hpp
    src/monsters/YoungGoldDragon.hpp
    src/monsters/Yochlol.hpp
    src/monsters/AwakenedPlants.hpp
    src/monsters/Larvae.hpp
)

# Create target
add_executable(itsamonster ${ITSAMONSTER_SOURCES})

target_include_directories(itsamonster PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Visual Studio (and other IDEs) – group by physical folders under src
# This replaces the default "Header Files"/"Source Files" buckets with the folder tree.
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    FILES ${ITSAMONSTER_SOURCES}
)

if(MSVC)
    # Disable RTTI
    target_compile_options(itsamonster PRIVATE /GR-)
    # Tell MSVC STL no exceptions. Must appear before including standard headers.
    target_compile_definitions(itsamonster PRIVATE _HAS_EXCEPTIONS=0)
    # (Do NOT write try/throw/catch; avoid dynamic_cast/typeid.)
else()
    # GCC / Clang / clang-cl (non-MSVC front-end)
    target_compile_options(itsamonster PRIVATE -fno-rtti -fno-exceptions)
endif()

