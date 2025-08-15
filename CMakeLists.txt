cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)

project(itsamonster)
enable_language(CXX)

# Optimize for speed over size in Release mode
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ot /Ob2 /DNDEBUG")
endif()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Explicit source list (core library sources excluding main)
set(ITSAMONSTER_LIB_SOURCES
    src/Action.cpp
    src/Action.hpp
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

# (Release optimization flags for both library and executable retained)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(itsamonster_lib PRIVATE /O2 /Ot /Ob2)
        target_compile_options(itsamonster PRIVATE /O2 /Ot /Ob2)
    else()
        target_compile_options(itsamonster_lib PRIVATE -O3)
        target_compile_options(itsamonster PRIVATE -O3)
    endif()
endif()

