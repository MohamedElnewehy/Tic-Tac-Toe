#include "overwrite_game.h"

bool canOverwrite(const GameState* game, const LastMoves* lastMoves, int row, int col) {
    if (game->currentPlayer == 'X') {
        if (row == lastMoves->lastPlayerORow && col == lastMoves->lastPlayerOCol) {
            return false;
        }
    } else {
        if (row == lastMoves->lastPlayerXRow && col == lastMoves->lastPlayerXCol) {
            return false;
        }
    }
    return true;
}

void startOverwriteGame() {
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

        if (row < 0 || row > 2 || col < 0 || col > 2) {
            cout << "Invalid input! Please enter values between 0-2.\n";
            continue;
        }

        if (!canOverwrite(&game, &lastMoves, row, col)) {
            cout << "Cannot overwrite opponent's last move! Choose another position.\n";
            continue;
        }

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

    cout << "Press Enter to return to main menu...";
    cin.ignore();
    cin.get();
}
