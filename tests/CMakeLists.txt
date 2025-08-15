
# Ensure googletest builds against the shared (dynamic) MSVC runtime (/MD,/MDd)
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use shared CRT" FORCE)
endif()

enable_testing()
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp ${CMAKE_SOURCE_DIR}/tests/test_utils/*.cpp)
if(TEST_SOURCES)
    add_executable(itsamonster_tests ${TEST_SOURCES})
    target_link_libraries(itsamonster_tests PRIVATE itsamonster_lib GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    target_include_directories(itsamonster_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
    # Re-enable RTTI (only) for tests (keep exceptions disabled unless you want them)
    if(MSVC)
        target_compile_options(itsamonster_tests PRIVATE /GR)
        target_compile_definitions(itsamonster_tests PRIVATE _HAS_EXCEPTIONS=0)
    else()
        target_compile_options(itsamonster_tests PRIVATE -frtti -fno-exceptions)
    endif()
    include(GoogleTest)
    gtest_discover_tests(itsamonster_tests DISCOVERY_MODE PRE_TEST)

    if(TARGET itsamonster_tests)
        target_compile_options(itsamonster_tests PRIVATE
                            $<$<CONFIG:Release>:/O2>
                            $<$<CONFIG:RelWithDebInfo>:/O2>
                            $<$<CONFIG:MinSizeRel>:/O1>
                            $<$<CONFIG:Debug>:/Od>)
    endif()
else()
    message(WARNING "ITSAMONSTER_ENABLE_TESTS=ON but no test sources found in tests/*.cpp")
endif()