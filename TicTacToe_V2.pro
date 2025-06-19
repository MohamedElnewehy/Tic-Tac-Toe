# TicTacToe_V2.pro

QT       += core gui widgets sql
CONFIG  += c++17
SOURCES += \
    GameHistoryManager.cpp \
    GameWindow.cpp \
    TicTacToeDB.cpp \
    ai_game.cpp \
    classic_game.cpp \
    historywidow.cpp \
    main.cpp \
    mainwindow.cpp \
    overwrite_game.cpp \
    sqlite3.c

HEADERS += \
    GameHistoryManager.h \
    GameWindow.h \
    Picosha2.h \
    TicTacToeDB.h \
    ai_game.h \
    classic_game.h \
    historywindow.h \
    mainwindow.h \
    overwrite_game.h \
    sqlite3.h

FORMS += mainwindow.ui

# Add Google Test integration
CONFIG += console testcase
TEMPLATE = app

# Test target
TESTSOURCES += \
    tests/main_test.cpp \
    tests/game_logic_tests/ai_game_test.cpp \
    tests/game_logic_tests/classic_game_test.cpp \
    tests/game_logic_tests/overwrite_game_test.cpp \
    tests/database_tests/tictactoe_db_test.cpp \
    tests/ui_tests/gamewindow_test.cpp

unix:!mac:!win32 {
    include(google_tests.pri)
}
