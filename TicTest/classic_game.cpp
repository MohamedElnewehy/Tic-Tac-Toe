#include "classic_game.h"

void initializeBoard(GameState* game) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            game->board[i][j] = ' ';
        }
    }
    game->currentPlayer = 'X';
    game->gameActive = true;
}

void printBoard(const GameState* game) {
    cout << "\n  0   1   2\n";
    for (int i = 0; i < 3; i++) {
        cout << i << " ";
        for (int j = 0; j < 3; j++) {
            cout << game->board[i][j];
            if (j < 2) cout << " | ";
        }
        cout << "\n";
        if (i < 2) cout << "  ---------\n";
    }
    cout << "\n";
}

bool makeMove(GameState* game, int row, int col, bool allowOverwrite) {
    if (row < 0 || row > 2 || col < 0 || col > 2) {
        cout << "Invalid position! Please enter values between 0-2.\n";
        return false;
    }

    if (!allowOverwrite && game->board[row][col] != ' ') {
        cout << "Position already taken! Choose another position.\n";
        return false;
    }

    game->board[row][col] = game->currentPlayer;
    return true;
}

bool checkWin(const GameState* game) {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (game->board[i][0] == game->currentPlayer &&
            game->board[i][1] == game->currentPlayer &&
            game->board[i][2] == game->currentPlayer) {
            return true;
        }
    }

    // Check columns
    for (int j = 0; j < 3; j++) {
        if (game->board[0][j] == game->currentPlayer &&
            game->board[1][j] == game->currentPlayer &&
            game->board[2][j] == game->currentPlayer) {
            return true;
        }
    }

    // Check diagonals
    if (game->board[0][0] == game->currentPlayer &&
        game->board[1][1] == game->currentPlayer &&
        game->board[2][2] == game->currentPlayer) {
        return true;
    }

    if (game->board[0][2] == game->currentPlayer &&
        game->board[1][1] == game->currentPlayer &&
        game->board[2][0] == game->currentPlayer) {
        return true;
    }

    return false;
}

bool checkDraw(const GameState* game) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game->board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

void switchPlayer(GameState* game) {
    game->currentPlayer = (game->currentPlayer == 'X') ? 'O' : 'X';
}

void startClassicTicTacToe() {
    GameState game;
    initializeBoard(&game);

    cout << "\n=== Classic Tic Tac Toe 1v1 Game ===\n";
    cout << "Rules: Players take turns, cannot overwrite existing moves\n";
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

    cout << "Press Enter to return to main menu...";
    cin.ignore();
    cin.get();
}
