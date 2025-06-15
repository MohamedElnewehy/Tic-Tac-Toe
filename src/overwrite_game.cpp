#include "overwrite_game.h"

OverwriteGame::OverwriteGame()
    : board(9, ' '), currentPlayer('X'), gameEnded(false), turnCount(0),
    cellTurn(9, 0), cellOwner(9, ' ')
{
}

OverwriteGame::~OverwriteGame()
{
}

void OverwriteGame::initializeGame()
{
    board = std::vector<char>(9, ' ');
    moveHistory.clear();
    gameEnded = false;
    currentPlayer = 'X';
    turnCount = 0;
    cellTurn = std::vector<int>(9, 0);
    cellOwner = std::vector<char>(9, ' ');
}

bool OverwriteGame::makeMove(int position, char player)
{
    if (gameEnded || position < 0 || position >= 9 || !isValidMove(position, player))
        return false;

    // Record the move
    board[position] = player;
    moveHistory.push_back(std::string(1, player) + std::to_string(position / 3) + std::to_string(position % 3));
    turnCount++;
    cellTurn[position] = turnCount;
    cellOwner[position] = player;

    // Check for win or draw
    if (checkWin(player)) {
        gameEnded = true;
    } else if (checkDraw()) {
        gameEnded = true;
    } else {
        switchPlayer();
    }

    return true;
}

bool OverwriteGame::isValidMove(int position, char player) const
{
    if (position < 0 || position >= 9)
        return false;

    // If cell is empty, move is always valid
    if (board[position] == ' ')
        return true;

    // If cell is occupied by the same player, move is invalid
    if (board[position] == player)
        return false;

    // Overwrite logic: can only overwrite if opponent has played after this cell
    return canOverwrite(position, player);
}

bool OverwriteGame::canOverwrite(int position, char player) const
{
    char opponent = (player == 'X') ? 'O' : 'X';

    // Check if the opponent has played at least one move AFTER this cell's turn
    for (int i = 0; i < 9; ++i) {
        if (i != position && cellOwner[i] == opponent && cellTurn[i] > cellTurn[position]) {
            return true;
        }
    }
    return false;
}

bool OverwriteGame::checkWin(char player) const
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

bool OverwriteGame::checkDraw() const
{
    for (int i = 0; i < 9; i++) {
        if (board[i] == ' ') {
            return false;
        }
    }
    return true;
}

void OverwriteGame::switchPlayer()
{
    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
}
