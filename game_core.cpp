#include "game_core.h"

void initializeBoard(GameState* game) {
    game->board = vector< vector<char> >(3, vector<char>(3, ' '));
    game->currentPlayer = 'X';
    game->gameActive = true;
}

void printBoard(const GameState* game) {
    cout << "\n";
    for (int i = 0; i < 3; ++i) {
        cout << " ";
        for (int j = 0; j < 3; ++j) {
            cout << game->board[i][j];
            if (j < 2) cout << " | ";
        }
        cout << "\n";
        if (i < 2) cout << "---+---+---\n";
    }
    cout << "\n";
}

bool checkWin(const GameState* game) {
    char player = game->currentPlayer;

    for (int i = 0; i < 3; ++i) {
        if ((game->board[i][0] == player &&
             game->board[i][1] == player &&
             game->board[i][2] == player) ||
            (game->board[0][i] == player &&
             game->board[1][i] == player &&
             game->board[2][i] == player)) {
            return true;
        }
    }

    if ((game->board[0][0] == player &&
         game->board[1][1] == player &&
         game->board[2][2] == player) ||
        (game->board[0][2] == player &&
         game->board[1][1] == player &&
         game->board[2][0] == player)) {
        return true;
    }

    return false;
}

bool checkDraw(const GameState* game) {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (game->board[i][j] == ' ')
                return false;
    return true;
}

void switchPlayer(GameState* game) {
    game->currentPlayer = (game->currentPlayer == 'X') ? 'O' : 'X';
}

bool isValidPosition(int row, int col) {
    return (row >= 0 && row < 3 && col >= 0 && col < 3);
}

bool makeMove(GameState* game, int row, int col, bool allowOverwrite) {
    if (!isValidPosition(row, col)) {
        cout << "Invalid move! Try again.\n";
        return false;
    }

    if (!allowOverwrite && game->board[row][col] != ' ') {
        cout << "Cell already occupied! Try again.\n";
        return false;
    }

    if (allowOverwrite && game->board[row][col] != ' ') {
        cout << "Overwriting cell (" << row << "," << col << ") that contained '"
             << game->board[row][col] << "'\n";
    }

    game->board[row][col] = game->currentPlayer;
    return true;
}

