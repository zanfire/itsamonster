cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)

project(itsamonster)
enable_language(CXX)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Explicit source list (core library sources excluding main)
set(ITSAMONSTER_LIB_SOURCES
    src/Logger.cpp
    src/Match.cpp
    src/Match.hpp
    src/Types.hpp
    src/actions/Action.cpp
    src/actions/Action.hpp
    src/actions/AttackAction.cpp
    src/actions/AttackAction.hpp
    src/actions/RechargeAction.cpp
    src/actions/RechargeAction.hpp
    src/core/Battlefield.cpp
    src/core/Battlefield.hpp
    src/core/Dice.cpp
    src/core/Dice.hpp
    src/core/AI.cpp
    src/core/AI.hpp
    src/core/CombatSystem.cpp
    src/core/CombatSystem.hpp
    src/reactions/Reaction.cpp
    src/reactions/Reaction.hpp
    src/monsters/Monster.cpp
    src/monsters/Monster.hpp
    src/monsters/YoungGoldDragon.hpp
    src/monsters/AcidFiend.hpp
    src/monsters/AwakenedPlants.hpp
    src/monsters/Larvae.hpp
)

add_library(itsamonster_lib ${ITSAMONSTER_LIB_SOURCES})
target_include_directories(itsamonster_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(itsamonster_lib PRIVATE /GR-)
    target_compile_definitions(itsamonster_lib PRIVATE _HAS_EXCEPTIONS=0)
else()
    target_compile_options(itsamonster_lib PRIVATE -fno-rtti -fno-exceptions)
endif()

# Main executable with only entry point
add_executable(itsamonster src/itsamonster.cpp)
target_link_libraries(itsamonster PRIVATE itsamonster_lib)
target_include_directories(itsamonster PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${ITSAMONSTER_LIB_SOURCES} src/itsamonster.cpp)

# ---- Testing (GoogleTest via submodule) ----
option(ITSAMONSTER_ENABLE_TESTS "Build unit tests" ON)

if(ITSAMONSTER_ENABLE_TESTS)
    if(EXISTS ${CMAKE_SOURCE_DIR}/extern/googletest/CMakeLists.txt)
        add_subdirectory(extern/googletest)
        add_subdirectory(tests)
    else()
        message(WARNING "googletest submodule not found at extern/googletest. Run: git submodule add https://github.com/google/googletest.git extern/googletest")
    endif()
endif()

# Configuration-specific tweaks
if(MSVC)
    target_compile_options(itsamonster_lib PRIVATE
                            $<$<CONFIG:Release>:/O2>
                            $<$<CONFIG:RelWithDebInfo>:/O2>
                            $<$<CONFIG:MinSizeRel>:/O1>
                            $<$<CONFIG:Debug>:/Od>)
    target_compile_options(itsamonster PRIVATE
                            $<$<CONFIG:Release>:/O2>
                            $<$<CONFIG:RelWithDebInfo>:/O2>
                            $<$<CONFIG:MinSizeRel>:/O1>
                            $<$<CONFIG:Debug>:/Od>)
endif()

