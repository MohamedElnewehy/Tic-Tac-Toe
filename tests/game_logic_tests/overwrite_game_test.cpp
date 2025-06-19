#include <gtest/gtest.h>
#include "overwrite_game.h"

class OverwriteGameTest : public ::testing::Test {
protected:
    void SetUp() override {
        game = new OverwriteGame();
    }

    void TearDown() override {
        delete game;
    }

    OverwriteGame* game;
};

// Test initialization
TEST_F(OverwriteGameTest, InitializationSetsUpBoardCorrectly) {
    EXPECT_EQ(game->getCurrentPlayer(), 'X');
    EXPECT_FALSE(game->isGameEnded());
    EXPECT_EQ(game->getTurnCount(), 0);
    
    // Check all cells are empty
    for (int i = 0; i < 9; i++) {
        EXPECT_EQ(game->getCellValue(i), ' ');
    }
}

// Test basic move functionality
TEST_F(OverwriteGameTest, BasicMoveWorks) {
    EXPECT_TRUE(game->makeMove(0, 'X'));
    EXPECT_EQ(game->getCellValue(0), 'X');
    EXPECT_EQ(game->getCurrentPlayer(), 'O');
    EXPECT_EQ(game->getTurnCount(), 1);
}

// Test invalid moves
TEST_F(OverwriteGameTest, InvalidMovesRejected) {
    // Out of bounds moves
    EXPECT_FALSE(game->makeMove(-1, 'X'));
    EXPECT_FALSE(game->makeMove(9, 'X'));
    
    // Wrong player's turn
    EXPECT_TRUE(game->makeMove(0, 'X'));
    EXPECT_FALSE(game->makeMove(1, 'X')); // X tries to go again
    EXPECT_EQ(game->getCurrentPlayer(), 'O'); // Should still be O's turn
}

// Test win detection - rows
TEST_F(OverwriteGameTest, DetectsRowWin) {
    EXPECT_TRUE(game->makeMove(0, 'X')); // X in top-left
    EXPECT_TRUE(game->makeMove(3, 'O')); // O in middle-left
    EXPECT_TRUE(game->makeMove(1, 'X')); // X in top-middle
    EXPECT_TRUE(game->makeMove(4, 'O')); // O in center
    EXPECT_TRUE(game->makeMove(2, 'X')); // X in top-right
    
    EXPECT_TRUE(game->isGameEnded());
    EXPECT_TRUE(game->checkWin('X'));
}

// Test win detection - columns
TEST_F(OverwriteGameTest, DetectsColumnWin) {
    EXPECT_TRUE(game->makeMove(0, 'X')); // X in top-left
    EXPECT_TRUE(game->makeMove(1, 'O')); // O in top-middle
    EXPECT_TRUE(game->makeMove(3, 'X')); // X in middle-left
    EXPECT_TRUE(game->makeMove(2, 'O')); // O in top-right
    EXPECT_TRUE(game->makeMove(6, 'X')); // X in bottom-left
    
    EXPECT_TRUE(game->isGameEnded());
    EXPECT_TRUE(game->checkWin('X'));
}

// Test win detection - diagonals
TEST_F(OverwriteGameTest, DetectsDiagonalWin) {
    EXPECT_TRUE(game->makeMove(0, 'X')); // X in top-left
    EXPECT_TRUE(game->makeMove(1, 'O')); // O in top-middle
    EXPECT_TRUE(game->makeMove(4, 'X')); // X in center
    EXPECT_TRUE(game->makeMove(2, 'O')); // O in top-right
    EXPECT_TRUE(game->makeMove(8, 'X')); // X in bottom-right
    
    EXPECT_TRUE(game->isGameEnded());
    EXPECT_TRUE(game->checkWin('X'));
}

// Test draw detection
TEST_F(OverwriteGameTest, DetectsDraw) {
    // Fill the board without a win
    EXPECT_TRUE(game->makeMove(0, 'X')); // X in top-left
    EXPECT_TRUE(game->makeMove(1, 'O')); // O in top-middle
    EXPECT_TRUE(game->makeMove(2, 'X')); // X in top-right
    EXPECT_TRUE(game->makeMove(4, 'O')); // O in center
    EXPECT_TRUE(game->makeMove(3, 'X')); // X in middle-left
    EXPECT_TRUE(game->makeMove(5, 'O')); // O in middle-right
    EXPECT_TRUE(game->makeMove(7, 'X')); // X in bottom-middle
    EXPECT_TRUE(game->makeMove(6, 'O')); // O in bottom-left
    EXPECT_TRUE(game->makeMove(8, 'X')); // X in bottom-right
    
    EXPECT_TRUE(game->isGameEnded());
    EXPECT_FALSE(game->checkWin('X'));
    EXPECT_FALSE(game->checkWin('O'));
    EXPECT_TRUE(game->checkDraw());
}

// Test overwrite functionality - cannot overwrite own cells
TEST_F(OverwriteGameTest, CannotOverwriteOwnCells) {
    EXPECT_TRUE(game->makeMove(0, 'X')); // X in top-left
    EXPECT_TRUE(game->makeMove(1, 'O')); // O in top-middle
    EXPECT_TRUE(game->makeMove(2, 'X')); // X in top-right
    
    // O tries to overwrite its own cell
    EXPECT_FALSE(game->makeMove(1, 'O'));
    EXPECT_EQ(game->getCellValue(1), 'O'); // Cell should remain O
    EXPECT_EQ(game->getCurrentPlayer(), 'O'); // Player should still be O
}

// Test overwrite functionality - cannot overwrite opponent's most recent move
TEST_F(OverwriteGameTest, CannotOverwriteOpponentMostRecentMove) {
    EXPECT_TRUE(game->makeMove(0, 'X')); // X in top-left
    
    // O tries to overwrite X's most recent move
    EXPECT_FALSE(game->makeMove(0, 'O'));
    EXPECT_EQ(game->getCellValue(0), 'X'); // Cell should remain X
    EXPECT_EQ(game->getCurrentPlayer(), 'O'); // Player should still be O
}

// Test overwrite functionality - can overwrite opponent's earlier moves
TEST_F(OverwriteGameTest, CanOverwriteOpponentEarlierMove) {
    // X plays at position 0
    EXPECT_TRUE(game->makeMove(0, 'X'));
    EXPECT_EQ(game->getCellValue(0), 'X');
    EXPECT_EQ(game->getCurrentPlayer(), 'O');
    
    // O plays at position 1
    EXPECT_TRUE(game->makeMove(1, 'O'));
    EXPECT_EQ(game->getCellValue(1), 'O');
    EXPECT_EQ(game->getCurrentPlayer(), 'X');
    
    // X plays at position 2
    EXPECT_TRUE(game->makeMove(2, 'X'));
    EXPECT_EQ(game->getCellValue(2), 'X');
    EXPECT_EQ(game->getCurrentPlayer(), 'O');
    
    // O should be able to overwrite X's move at position 0 because X has played since then
    EXPECT_TRUE(game->makeMove(0, 'O'));
    EXPECT_EQ(game->getCellValue(0), 'O');
    EXPECT_EQ(game->getCurrentPlayer(), 'X');
    
    // X should NOT be able to overwrite O's most recent move at position 0
    EXPECT_FALSE(game->makeMove(0, 'X'));
    EXPECT_EQ(game->getCellValue(0), 'O'); // Cell should remain O
    EXPECT_EQ(game->getCurrentPlayer(), 'X'); // Player should still be X (turn not changed)
}

// Test win after overwrite
TEST_F(OverwriteGameTest, WinAfterOverwrite) {
    // X plays at position 0
    EXPECT_TRUE(game->makeMove(0, 'X'));
    
    // O plays at position 3
    EXPECT_TRUE(game->makeMove(3, 'O'));
    
    // X plays at position 1
    EXPECT_TRUE(game->makeMove(1, 'X'));
    
    // O plays at position 4
    EXPECT_TRUE(game->makeMove(4, 'O'));
    
    // X plays at position 6
    EXPECT_TRUE(game->makeMove(6, 'X'));
    
    // O plays at position 5
    EXPECT_TRUE(game->makeMove(5, 'O'));
    
    // X plays at position 7
    EXPECT_TRUE(game->makeMove(7, 'X'));
    
    // O overwrites X's move at position 0 to create a winning diagonal (0-4-8)
    EXPECT_TRUE(game->makeMove(0, 'O'));
    
    // O plays at position 8 to win
    EXPECT_TRUE(game->makeMove(8, 'O'));
    
    EXPECT_TRUE(game->isGameEnded());
    EXPECT_TRUE(game->checkWin('O'));
}

// Test move history tracking
TEST_F(OverwriteGameTest, MoveHistoryTracking) {
    EXPECT_TRUE(game->makeMove(0, 'X')); // X in top-left
    EXPECT_TRUE(game->makeMove(1, 'O')); // O in top-middle
    EXPECT_TRUE(game->makeMove(2, 'X')); // X in top-right
    
    auto history = game->getMoveHistory();
    EXPECT_EQ(history.size(), 3);
    EXPECT_EQ(history[0], "X00"); // X at row 0, col 0
    EXPECT_EQ(history[1], "O01"); // O at row 0, col 1
    EXPECT_EQ(history[2], "X02"); // X at row 0, col 2
    
    // Test overwrite is recorded in history
    EXPECT_TRUE(game->makeMove(4, 'O')); // O in center
    EXPECT_TRUE(game->makeMove(0, 'X')); // X overwrites top-left
    
    history = game->getMoveHistory();
    EXPECT_EQ(history.size(), 5);
    EXPECT_EQ(history[3], "O11"); // O at row 1, col 1
    EXPECT_EQ(history[4], "X00"); // X at row 0, col 0 (overwrite)
}

// Test game reset
TEST_F(OverwriteGameTest, GameReset) {
    // Make some moves
    EXPECT_TRUE(game->makeMove(0, 'X'));
    EXPECT_TRUE(game->makeMove(1, 'O'));
    
    // Reset the game
    game->initializeGame();
    
    // Check everything is reset
    EXPECT_EQ(game->getCurrentPlayer(), 'X');
    EXPECT_FALSE(game->isGameEnded());
    EXPECT_EQ(game->getTurnCount(), 0);
    
    // Check all cells are empty
    for (int i = 0; i < 9; i++) {
        EXPECT_EQ(game->getCellValue(i), ' ');
    }
    
    // Check move history is cleared
    EXPECT_EQ(game->getMoveHistory().size(), 0);
}

// Test complex overwrite scenario
TEST_F(OverwriteGameTest, ComplexOverwriteScenario) {
    // X plays at position 0
    EXPECT_TRUE(game->makeMove(0, 'X'));
    
    // O plays at position 1
    EXPECT_TRUE(game->makeMove(1, 'O'));
    
    // X plays at position 2
    EXPECT_TRUE(game->makeMove(2, 'X'));
    
    // O plays at position 3
    EXPECT_TRUE(game->makeMove(3, 'O'));
    
    // X plays at position 4
    EXPECT_TRUE(game->makeMove(4, 'X'));
    
    // O plays at position 5
    EXPECT_TRUE(game->makeMove(5, 'O'));
    
    // X plays at position 6
    EXPECT_TRUE(game->makeMove(6, 'X'));
    
    // O can overwrite X's move at position 0 (X's first move)
    EXPECT_TRUE(game->makeMove(0, 'O'));
    
    // X can overwrite O's move at position 1 (O's first move)
    EXPECT_TRUE(game->makeMove(1, 'X'));
    
    // O can overwrite X's move at position 2 (X's second move)
    EXPECT_TRUE(game->makeMove(2, 'O'));
    
    // Check the final board state
    EXPECT_EQ(game->getCellValue(0), 'O');
    EXPECT_EQ(game->getCellValue(1), 'X');
    EXPECT_EQ(game->getCellValue(2), 'O');
    EXPECT_EQ(game->getCellValue(3), 'O');
    EXPECT_EQ(game->getCellValue(4), 'X');
    EXPECT_EQ(game->getCellValue(5), 'O');
    EXPECT_EQ(game->getCellValue(6), 'X');
    
    // Check if O has won with a diagonal (0-4-8)
    EXPECT_FALSE(game->checkWin('O')); // Not yet, need position 8
    
    // X plays at position 7
    EXPECT_TRUE(game->makeMove(7, 'X'));
    
    // O plays at position 8 to win with a diagonal
    EXPECT_TRUE(game->makeMove(8, 'O'));
    
    EXPECT_TRUE(game->isGameEnded());
    EXPECT_TRUE(game->checkWin('O'));
}

// Test edge case: multiple overwrites on same position
TEST_F(OverwriteGameTest, MultipleOverwritesSamePosition) {
    // X plays at position 0
    EXPECT_TRUE(game->makeMove(0, 'X'));
    
    // O plays at position 1
    EXPECT_TRUE(game->makeMove(1, 'O'));
    
    // X plays at position 2
    EXPECT_TRUE(game->makeMove(2, 'X'));
    
    // O overwrites position 0
    EXPECT_TRUE(game->makeMove(0, 'O'));
    
    // X plays at position 3
    EXPECT_TRUE(game->makeMove(3, 'X'));
    
    // O plays at position 4
    EXPECT_TRUE(game->makeMove(4, 'O'));
    
    // X overwrites position 0 again
    EXPECT_TRUE(game->makeMove(0, 'X'));
    EXPECT_EQ(game->getCellValue(0), 'X');
}

// Test overwrite validation with turn tracking
TEST_F(OverwriteGameTest, OverwriteValidationWithTurnTracking) {
    // Create a scenario with multiple moves
    EXPECT_TRUE(game->makeMove(0, 'X')); // Turn 1
    EXPECT_TRUE(game->makeMove(1, 'O')); // Turn 2
    EXPECT_TRUE(game->makeMove(2, 'X')); // Turn 3
    EXPECT_TRUE(game->makeMove(3, 'O')); // Turn 4
    EXPECT_TRUE(game->makeMove(4, 'X')); // Turn 5
    
    // O should be able to overwrite any X move except the most recent (turn 5)
    EXPECT_TRUE(game->makeMove(0, 'O'));  // Can overwrite turn 1
    EXPECT_TRUE(game->makeMove(2, 'X'));  // X can overwrite turn 3 (its own earlier move after O played)
    
    // Check turn count increases correctly
    EXPECT_EQ(game->getTurnCount(), 7);
}
