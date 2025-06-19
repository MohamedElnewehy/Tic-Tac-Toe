#include <gtest/gtest.h>
#include "ai_game.h"

class AIGameTest : public ::testing::Test {
protected:
    void SetUp() override {
        easyAI = new AIGame(1);
        mediumAI = new AIGame(2);
        hardAI = new AIGame(3);
    }

    void TearDown() override {
        delete easyAI;
        delete mediumAI;
        delete hardAI;
    }

    AIGame* easyAI;
    AIGame* mediumAI;
    AIGame* hardAI;
};

TEST_F(AIGameTest, DifficultyIsSetCorrectly) {
    EXPECT_EQ(easyAI->getDifficulty(), 1);
    EXPECT_EQ(mediumAI->getDifficulty(), 2);
    EXPECT_EQ(hardAI->getDifficulty(), 3);
}

TEST_F(AIGameTest, EasyAIMakesValidMoves) {
    std::vector<char> board(9, ' ');
    int move = easyAI->getAIMove(board, 'O', 'X');
    EXPECT_GE(move, 0);
    EXPECT_LT(move, 9);
    EXPECT_EQ(board[move], ' '); // Should be empty cell
}

TEST_F(AIGameTest, HardAIBlocksWinningMove) {
    std::vector<char> board = {
        'X', 'X', ' ',  // Row 0: X X _
        ' ', 'O', ' ',  // Row 1: _ O _
        ' ', ' ', ' '   // Row 2: _ _ _
    };
    int move = hardAI->getAIMove(board, 'O', 'X');
    EXPECT_EQ(move, 2); // Should block at position 2
}

TEST_F(AIGameTest, HardAITakesWinningMove) {
    std::vector<char> board = {
        'O', 'O', ' ',  // Row 0: O O _
        'X', 'X', ' ',  // Row 1: X X _
        ' ', ' ', ' '   // Row 2: _ _ _
    };
    int move = hardAI->getAIMove(board, 'O', 'X');
    EXPECT_EQ(move, 2); // Should win at position 2
}

TEST_F(AIGameTest, EvaluateBoardCorrectly) {
    // AI wins
    std::vector<char> winBoard = {
        'O', 'O', 'O',
        'X', 'X', ' ',
        ' ', ' ', ' '
    };
    EXPECT_EQ(hardAI->evaluate(winBoard, 'O', 'X'), 10);

    // Human wins
    std::vector<char> loseBoard = {
        'X', 'X', 'X',
        'O', 'O', ' ',
        ' ', ' ', ' '
    };
    EXPECT_EQ(hardAI->evaluate(loseBoard, 'O', 'X'), -10);

    // Draw/ongoing
    std::vector<char> drawBoard = {
        'X', 'O', 'X',
        'O', 'X', 'O',
        'O', 'X', 'O'
    };
    EXPECT_EQ(hardAI->evaluate(drawBoard, 'O', 'X'), 0);
}

TEST_F(AIGameTest, AvailableMovesCorrect) {
    std::vector<char> board = {
        'X', ' ', 'O',
        ' ', 'X', ' ',
        'O', ' ', ' '
    };
    std::vector<int> moves = hardAI->getAvailableMoves(board);
    std::vector<int> expected = {1, 3, 5, 7, 8};
    EXPECT_EQ(moves, expected);
}
