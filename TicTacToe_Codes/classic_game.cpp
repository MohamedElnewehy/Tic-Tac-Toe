#include "classic_game.h"

void startClassicGame() {
    GameState game;
    initializeBoard(&game);

    cout << "\n=== Classic Tic Tac Toe 1v1 Game ===\n";
    cout << "Rules: Cannot overwrite occupied cells\n";
    printBoard(&game);

    while (game.gameActive) {
        int row, col;
        cout << "Player " << game.currentPlayer << ", enter row and column (0-2): ";
        cin >> row >> col;

        if (!makeMove(&game, row, col, false)) continue;

        printBoard(&game);

        if (checkWin(&game)) {
            cout << "Player " << game.currentPlayer << " wins!\n";
            game.gameActive = false;
            break;
        }

        if (checkDraw(&game)) {
            cout << "Game ended in a draw!\n";
            game.gameActive = false;
            break;
        }

        switchPlayer(&game);
    }
}

