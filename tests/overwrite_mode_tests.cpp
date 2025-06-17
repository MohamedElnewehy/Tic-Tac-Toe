#include "gtest/gtest.h"
#include <iostream>
#include <vector>
using namespace std;

typedef struct {
    vector< vector<char> > board;
    char currentPlayer;
    bool gameActive;
} GameState;

// Structure to track last moves
struct LastMoves {
    int lastPlayerXRow = -1;
    int lastPlayerXCol = -1;
    int lastPlayerORow = -1;
    int lastPlayerOCol = -1;
};

void initializeBoard(GameState* game) {
    game->board = vector< vector<char> >(3, vector<char>(3, ' '));
    game->currentPlayer = 'X';
    game->gameActive = true;
}

void switchPlayer(GameState* game) {
    game->currentPlayer = (game->currentPlayer == 'X') ? 'O' : 'X';
}

bool canOverwrite(const GameState* game, const LastMoves* lastMoves, int row, int col) {
    // Check if this position is the opponent's last move
    if (game->currentPlayer == 'X') {
        // Player X cannot overwrite Player O's last move
        if (row == lastMoves->lastPlayerORow && col == lastMoves->lastPlayerOCol) {
            return false;
        }
    } else {
        // Player O cannot overwrite Player X's last move
        if (row == lastMoves->lastPlayerXRow && col == lastMoves->lastPlayerXCol) {
            return false;
        }
    }
    return true;
}

TEST(OverwriteModeTests, OverwriteValidation){
    GameState game;
    game.board = vector<vector<char>>(3, vector<char>(3, ' '));
    game.currentPlayer = 'X';
    game.gameActive = true;
    
    LastMoves moves = {1,0,2,2};//last X move was row 1 column 0 
                               //last O move was row 2 column 2
    game.board[1][0] = 'X';
    game.board[2][2] = 'O';
    
    EXPECT_FALSE(canOverwrite(&game,&moves,2,2));//X overwriting the last place 
                                                 //O was written in
    game.currentPlayer = 'O';
    EXPECT_FALSE(canOverwrite(&game,&moves,1,0));//O overwriting the last place
                                                 //X was written in

    EXPECT_TRUE(canOverwrite(&game,&moves,1,2));//O getting written somewhere
                                                //where X wasnt written in
}

int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
