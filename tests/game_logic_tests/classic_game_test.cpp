#include <gtest/gtest.h>
#include "classic_game.h"

class ClassicGameTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = new ClassicGame();
    }

    void TearDown() override {
        delete game;
    }

    ClassicGame* game;
};

TEST_F(ClassicGameTest, GameInitializesCorrectly) {
    EXPECT_EQ(game->getCurrentPlayer(), 'X');
    EXPECT_FALSE(game->isGameEnded());
    for (int i = 0; i < 9; i++) {
        EXPECT_EQ(game->getCellValue(i), ' ');
    }
}

TEST_F(ClassicGameTest, ValidMovesWork) {
    EXPECT_TRUE(game->makeMove(0, 'X'));
    EXPECT_EQ(game->getCellValue(0), 'X');
    EXPECT_EQ(game->getCurrentPlayer(), 'O');
}

TEST_F(ClassicGameTest, RowWinDetected) {
    game->makeMove(0, 'X');
    game->makeMove(3, 'O');
    game->makeMove(1, 'X');
    game->makeMove(4, 'O');
    game->makeMove(2, 'X');
    
    EXPECT_TRUE(game->isGameEnded());
    EXPECT_TRUE(game->checkWin('X'));
}

TEST_F(ClassicGameTest, DrawDetected) {
    // Create a draw scenario
    game->makeMove(0, 'X'); // X
    game->makeMove(1, 'O'); // X O
    game->makeMove(2, 'X'); // X O X
    game->makeMove(3, 'O'); // X O X O
    game->makeMove(4, 'X'); // X O X O X
    game->makeMove(5, 'O'); // X O X O X O
    game->makeMove(7, 'X'); // X O X O X O _ X
    game->makeMove(6, 'O'); // X O X O X O O X
    game->makeMove(8, 'X'); // X O X O X O O X X
    
    EXPECT_TRUE(game->isGameEnded());
    EXPECT_TRUE(game->checkDraw());
}
