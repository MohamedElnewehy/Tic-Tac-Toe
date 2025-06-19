cmake_minimum_required(VERSION 3.16)
project(AdvancedTicTacToe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
include(CTest)
include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

# Collect all test sources
file(GLOB_RECURSE TEST_SOURCES
  tests/*.cpp
)

# Define the test executable
add_executable(TicTacToe_Tests
  ${TEST_SOURCES}
  GameHistoryManager.cpp
  GameWindow.cpp
  TicTacToeDB.cpp
  ai_game.cpp
  classic_game.cpp
  historywidow.cpp
  mainwindow.cpp
  overwrite_game.cpp
  sqlite3.c
)

# Include your project source directory
target_include_directories(TicTacToe_Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Add Qt modules (✅ Include Qt6::Test)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Test)

# Link all required libraries
target_link_libraries(TicTacToe_Tests
  PRIVATE
    GTest::gtest
    GTest::gtest_main
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Test            # ✅ This fixes the 'QTest' not found error
)

# Register GoogleTest with CTest
enable_testing()
include(GoogleTest)
gtest_discover_tests(TicTacToe_Tests)
