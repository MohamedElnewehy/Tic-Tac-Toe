#include "overwrite_game.h"

void startOverwriteGame() {
    GameState game;
    initializeBoard(&game);

    cout << "\n=== Overwrite Mode Tic Tac Toe 1v1 Game ===\n";
    cout << "Rules: You can overwrite any cell on the board\n";
    printBoard(&game);

    int moveCount = 0;
    const int maxMoves = 50;

    while (game.gameActive && moveCount < maxMoves) {
        int row, col;
        cout << "Player " << game.currentPlayer << ", enter row and column (0-2): ";
        cin >> row >> col;

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
}

