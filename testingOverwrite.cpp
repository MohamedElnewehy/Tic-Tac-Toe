#include "overwrite_game.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>
#include "game_core.h"
using namespace std;




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

/*void startOverwriteGame() {
    GameState game;
    LastMoves lastMoves;
    initializeBoard(&game);

    cout << "\n=== Overwrite Mode Tic Tac Toe 1v1 Game ===\n";
    cout << "Rules: You can overwrite any cell except opponent's last move\n";
    printBoard(&game);

    int moveCount = 0;
    const int maxMoves = 50;

    while (game.gameActive && moveCount < maxMoves) {
        int row, col;
        cout << "Player " << game.currentPlayer << ", enter row and column (0-2): ";
        cin >> row >> col;

        // Validate input range
        if (row < 0 || row > 2 || col < 0 || col > 2) {
            cout << "Invalid input! Please enter values between 0-2.\n";
            continue;
        }

        // Check if overwrite is allowed
        if (!canOverwrite(&game, &lastMoves, row, col)) {
            cout << "Cannot overwrite opponent's last move! Choose another position.\n";
            continue;
        }

        // Store the current move before making it
        if (game.currentPlayer == 'X') {
            lastMoves.lastPlayerXRow = row;
            lastMoves.lastPlayerXCol = col;
        } else {
            lastMoves.lastPlayerORow = row;
            lastMoves.lastPlayerOCol = col;
        }

        if (!makeMove(&game, row, col, true)) continue;

        moveCount++;
        printBoard(&game);

        if (checkWin(&game)) {
            cout << "Player " << game.currentPlayer << " wins!\n";
            game.gameActive = false;
            break;
        }

        if (moveCount >= 6 && checkDraw(&game)) {
            cout << "Game ended in a draw!\n";
            game.gameActive = false;
            break;
        }

        if (moveCount >= maxMoves) {
            cout << "Game ended due to move limit!\n";
            game.gameActive = false;
            break;
        }

        switchPlayer(&game);
    }
}*/
/*testing part*/
TEST(overwrite,validity){
	GameState State;
    State.board = vector<vector<char>>(3, vector<char>(3, ' '));
    State.currentPlayer = 'X';
    State.gameActive = true;
LastMoves Moves = {1,0,2,2};//last X move was row 1 column 0 
			    //last O move was row 2 column 2
State.board[1][0] = 'X';
State.board[2][2] = 'O';
EXPECT_FALSE(canOverwrite(&State,&Moves,2,2));//X overwriting the last place 
					     //O was written in
State.currentPlayer = 'O';
EXPECT_FALSE(canOverwrite(&State,&Moves,1,0));//O overwriting the last place
					      //X was written in

EXPECT_TRUE(canOverwrite(&State,&Moves,1,2));//O getting written somewhere
					     //where X wasnt written in
}


int main(int argc, char** argv){
testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}
