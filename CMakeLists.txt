cmake_minimum_required(VERSION 3.16)
project(TicTacToeTests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Debug by default for testing
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add compile flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(Threads REQUIRED)

# Download and configure Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9fd7188fa9e0a66c7d4a5b1.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create build directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Core Game Logic Tests
add_executable(core_game_tests
    tests/core_game_tests.cpp
)
target_link_libraries(core_game_tests
    gtest
    gtest_main
    ${CMAKE_THREAD_LIBS_INIT}
)
set_target_properties(core_game_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Overwrite Mode Tests
add_executable(overwrite_mode_tests
    tests/overwrite_mode_tests.cpp
)
target_link_libraries(overwrite_mode_tests
    gtest
    gtest_main
    ${CMAKE_THREAD_LIBS_INIT}
)
set_target_properties(overwrite_mode_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# AI Engine Tests
add_executable(ai_engine_tests
    tests/ai_engine_tests.cpp
)
target_link_libraries(ai_engine_tests
    gtest
    gtest_main
    ${CMAKE_THREAD_LIBS_INIT}
)
set_target_properties(ai_engine_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Database Tests
add_executable(database_tests
    tests/database_tests.cpp
)
target_link_libraries(database_tests
    gtest
    gtest_main
    ${CMAKE_THREAD_LIBS_INIT}
)
set_target_properties(database_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# All Tests Runner
add_executable(all_tests
    tests/test_runner.cpp
    tests/core_game_tests.cpp
    tests/overwrite_mode_tests.cpp
    tests/ai_engine_tests.cpp
    tests/database_tests.cpp
)
target_link_libraries(all_tests
    gtest
    ${CMAKE_THREAD_LIBS_INIT}
)
set_target_properties(all_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Enable testing
enable_testing()

# Add tests to CTest
add_test(NAME CoreGameLogic COMMAND core_game_tests)
add_test(NAME OverwriteMode COMMAND overwrite_mode_tests)
add_test(NAME AIEngine COMMAND ai_engine_tests)
add_test(NAME DatabaseOperations COMMAND database_tests)
add_test(NAME AllTestsSuite COMMAND all_tests)

# Set test properties
set_tests_properties(CoreGameLogic PROPERTIES TIMEOUT 30)
set_tests_properties(OverwriteMode PROPERTIES TIMEOUT 30)
set_tests_properties(AIEngine PROPERTIES TIMEOUT 60)
set_tests_properties(DatabaseOperations PROPERTIES TIMEOUT 30)
set_tests_properties(AllTestsSuite PROPERTIES TIMEOUT 120)

# Custom target for running all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose --parallel 4
    DEPENDS core_game_tests overwrite_mode_tests ai_engine_tests database_tests all_tests
    COMMENT "Running all Tic Tac Toe tests"
)

# Coverage target (if supported)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
        
        add_custom_target(coverage
            COMMAND lcov --directory . --capture --output-file coverage.info
            COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND lcov --list coverage.info
            COMMAND genhtml -o coverage coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
    endif()
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "=== Tic Tac Toe Test Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Test executables will be created in: ${CMAKE_BINARY_DIR}/bin/")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  core_game_tests      - Core game logic tests")
message(STATUS "  overwrite_mode_tests - Overwrite mode tests")
message(STATUS "  ai_engine_tests      - AI engine tests")
message(STATUS "  database_tests       - Database tests")
message(STATUS "  all_tests            - All tests combined")
message(STATUS "  run_tests            - Run all tests with CTest")
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_COMPILER_IS_GNUCXX)
message(STATUS "  coverage             - Generate coverage report")
endif()
message(STATUS "")
