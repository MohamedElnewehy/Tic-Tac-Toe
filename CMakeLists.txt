cmake_minimum_required(VERSION 3.16)
project(TicTacToe_V2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Sql REQUIRED)

# Source files
set(SOURCES
    GameHistoryManager.cpp
    GameWindow.cpp
    TicTacToeDB.cpp
    ai_game.cpp
    classic_game.cpp
    historywidow.cpp
    main.cpp
    mainwindow.cpp
    overwrite_game.cpp
    sqlite3.c
)
set(HEADERS
    GameHistoryManager.h
    GameWindow.h
    TicTacToeDB.h
    ai_game.h
    classic_game.h
    historywindow.h
    mainwindow.h
    overwrite_game.h
    sqlite3.h
)
set(FORMS mainwindow.ui)

add_executable(TicTacToe_V2 ${SOURCES} ${HEADERS} ${FORMS})
target_link_libraries(TicTacToe_V2 PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)

# GoogleTest via FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)
