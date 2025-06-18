#include "classic_game.h"

ClassicGame::ClassicGame()
    : board(9, ' '), currentPlayer('X'), gameEnded(false)
{
}

ClassicGame::~ClassicGame()
{
}

void ClassicGame::initializeGame()
{
    board = std::vector<char>(9, ' ');
    moveHistory.clear();
    gameEnded = false;
    currentPlayer = 'X';
}

bool ClassicGame::makeMove(int position, char player)
{
    if (gameEnded || position < 0 || position >= 9 || !isValidMove(position))
        return false;

    board[position] = player;
    moveHistory.push_back(std::string(1, player) + std::to_string(position / 3) + std::to_string(position % 3));

    if (checkWin(player)) {
        gameEnded = true;
    } else if (checkDraw()) {
        gameEnded = true;
    } else {
        switchPlayer();
    }

    return true;
}

bool ClassicGame::isValidMove(int position) const
{
    if (position < 0 || position >= 9)
        return false;

    // Regular game: only empty cells are valid
    return board[position] == ' ';
}

bool ClassicGame::checkWin(char player) const
{
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i*3] == player && board[i*3+1] == player && board[i*3+2] == player) {
            return true;
        }
    }

    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[i] == player && board[i+3] == player && board[i+6] == player) {
            return true;
        }
    }

    // Check diagonals
    if (board[0] == player && board[4] == player && board[8] == player) {
        return true;
    }
    if (board[2] == player && board[4] == player && board[6] == player) {
        return true;
    }

    return false;
}

bool ClassicGame::checkDraw() const
{
    for (int i = 0; i < 9; i++) {
        if (board[i] == ' ') {
            return false;
        }
    }
    return true;
}

void ClassicGame::switchPlayer()
{
    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
}

// Legacy function for compatibility
void startClassicTicTacToe()
{
    // This function is now handled by GameWindow
    // Implementation moved to MainWindow for better integration
}
