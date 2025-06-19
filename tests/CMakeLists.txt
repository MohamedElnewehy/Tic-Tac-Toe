# Test sources
set(TEST_SOURCES
    main_test.cpp
    game_logic_tests/ai_game_test.cpp
    game_logic_tests/classic_game_test.cpp
    game_logic_tests/overwrite_game_test.cpp
    database_tests/tictactoe_db_test.cpp
    ui_tests/gamewindow_test.cpp
)

# Create the test executable
add_executable(TicTacToe_Tests ${TEST_SOURCES})

# Link with Google Test, Qt, and project sources
target_link_libraries(TicTacToe_Tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)

# Include the project source directory
target_include_directories(TicTacToe_Tests PRIVATE ${CMAKE_SOURCE_DIR})

# Add all source files except main.cpp for testing
target_sources(TicTacToe_Tests PRIVATE
    ${CMAKE_SOURCE_DIR}/GameHistoryManager.cpp
    ${CMAKE_SOURCE_DIR}/GameWindow.cpp
    ${CMAKE_SOURCE_DIR}/TicTacToeDB.cpp
    ${CMAKE_SOURCE_DIR}/ai_game.cpp
    ${CMAKE_SOURCE_DIR}/classic_game.cpp
    ${CMAKE_SOURCE_DIR}/historywidow.cpp
    ${CMAKE_SOURCE_DIR}/mainwindow.cpp
    ${CMAKE_SOURCE_DIR}/overwrite_game.cpp
    ${CMAKE_SOURCE_DIR}/sqlite3.c
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(TicTacToe_Tests)
