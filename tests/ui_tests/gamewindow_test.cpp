#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include "GameWindow.h"

class GameWindowTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        if (!QApplication::instance()) {
            int argc = 0;
            char** argv = nullptr;
            app = new QApplication(argc, argv);
        }
    }

    void SetUp() override {
        classicWindow = new GameWindow("Classic Mode");
        overwriteWindow = new GameWindow("Overwrite Mode");
        aiWindow = new GameWindow("AI Easy");
    }

    void TearDown() override {
        delete classicWindow;
        delete overwriteWindow;
        delete aiWindow;
    }

    static QApplication* app;
    GameWindow* classicWindow;
    GameWindow* overwriteWindow;
    GameWindow* aiWindow;
};

QApplication* GameWindowTest::app = nullptr;

TEST_F(GameWindowTest, WindowInitializesCorrectly) {
    EXPECT_FALSE(classicWindow->isGameEnded());
    EXPECT_EQ(classicWindow->getCurrentPlayer(), 'X');
    
    // Check all cells are empty
    for (int i = 0; i < 9; i++) {
        EXPECT_EQ(classicWindow->getCellValue(i), ' ');
    }
}

TEST_F(GameWindowTest, ClassicModeValidMoves) {
    // Valid move on empty cell
    EXPECT_TRUE(classicWindow->makeMove(0, 'X'));
    EXPECT_EQ(classicWindow->getCellValue(0), 'X');
    EXPECT_EQ(classicWindow->getCurrentPlayer(), 'O');
    
    // Invalid move on occupied cell
    EXPECT_FALSE(classicWindow->makeMove(0, 'O'));
    EXPECT_EQ(classicWindow->getCellValue(0), 'X'); // Should remain X
}

TEST_F(GameWindowTest, WinDetectionWorks) {
    // Create winning condition
    classicWindow->makeMove(0, 'X'); // X _ _
    classicWindow->makeMove(3, 'O'); // _ O _
    classicWindow->makeMove(1, 'X'); // _ _ _
    classicWindow->makeMove(4, 'O');
    classicWindow->makeMove(2, 'X'); // Should win
    
    EXPECT_TRUE(classicWindow->isGameEnded());
    EXPECT_TRUE(classicWindow->checkWin('X'));
}

TEST_F(GameWindowTest, AIGameInitializesCorrectly) {
    EXPECT_FALSE(aiWindow->isGameEnded());
    EXPECT_TRUE(aiWindow->isAIGame());
    EXPECT_EQ(aiWindow->getAIDifficulty(), 1); // Easy
}
