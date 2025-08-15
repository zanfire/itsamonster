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
    src/Battlefield.cpp
    src/Battlefield.hpp
    src/Logger.cpp
    src/Match.cpp
    src/Match.hpp
    src/Monster.cpp
    src/Monster.hpp
    src/Types.hpp
    src/Dice.cpp
    src/Dice.hpp
    src/actions/Action.cpp
    src/actions/Action.hpp
    src/actions/AttackAction.cpp
    src/actions/AttackAction.hpp
    src/actions/RechargeAction.cpp
    src/actions/RechargeAction.hpp
    src/monsters/YoungGoldDragon.hpp
    src/monsters/Yochlol.hpp
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
    # Expect googletest to be present as a git submodule under extern/googletest
    # Add instructions in README if not found.
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest/CMakeLists.txt)
        # Ensure googletest builds against the shared (dynamic) MSVC runtime (/MD,/MDd)
        if(MSVC)
            set(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use shared CRT" FORCE)
        endif()
        add_subdirectory(extern/googletest)
        enable_testing()
        file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
        if(TEST_SOURCES)
            add_executable(itsamonster_tests ${TEST_SOURCES})
            target_link_libraries(itsamonster_tests PRIVATE itsamonster_lib GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
            target_include_directories(itsamonster_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
            # Re-enable RTTI (only) for tests (keep exceptions disabled unless you want them)
            if(MSVC)
                target_compile_options(itsamonster_tests PRIVATE /GR)
                target_compile_definitions(itsamonster_tests PRIVATE _HAS_EXCEPTIONS=0)
            else()
                target_compile_options(itsamonster_tests PRIVATE -frtti -fno-exceptions)
            endif()
            include(GoogleTest)
            gtest_discover_tests(itsamonster_tests DISCOVERY_MODE PRE_TEST)
        else()
            message(WARNING "ITSAMONSTER_ENABLE_TESTS=ON but no test sources found in tests/*.cpp")
        endif()
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
    if(TARGET itsamonster_tests)
        target_compile_options(itsamonster_tests PRIVATE
                            $<$<CONFIG:Release>:/O2>
                            $<$<CONFIG:RelWithDebInfo>:/O2>
                            $<$<CONFIG:MinSizeRel>:/O1>
                            $<$<CONFIG:Debug>:/Od>)
    endif()

endif()

