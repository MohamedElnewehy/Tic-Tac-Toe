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

# Set sources for the main application (optional if you're only building tests here)
# You can also skip this if .pro handles the actual build outside CI

# Add test target
file(GLOB_RECURSE TEST_SOURCES
  tests/*.cpp
)

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

target_include_directories(TicTacToe_Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

target_link_libraries(TicTacToe_Tests
  PRIVATE
    GTest::gtest
    GTest::gtest_main
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(TicTacToe_Tests)
