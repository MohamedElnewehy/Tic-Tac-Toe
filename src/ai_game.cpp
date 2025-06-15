#include "ai_game.h"
#include <algorithm>

AIGame::AIGame(int difficulty) : aiDifficulty(difficulty)
{
}

AIGame::~AIGame()
{
}

int AIGame::getAIMove(const std::vector<char>& board, char aiPlayer, char humanPlayer)
{
    switch (aiDifficulty) {
    case 1: return getEasyAIMove(board);
    case 2: return getMediumAIMove(board, aiPlayer, humanPlayer);
    case 3: return getHardAIMove(board, aiPlayer, humanPlayer);
    default: return getEasyAIMove(board);
    }
}

int AIGame::getEasyAIMove(const std::vector<char>& board)
{
    std::vector<int> availableMoves = getAvailableMoves(board);

    if (availableMoves.empty()) return -1;

    int randomIndex = QRandomGenerator::global()->bounded(availableMoves.size());
    return availableMoves[randomIndex];
}

int AIGame::getMediumAIMove(const std::vector<char>& board, char aiPlayer, char humanPlayer)
{
    // Medium AI: 70% optimal, 30% random
    if (QRandomGenerator::global()->bounded(100) < 70) {
        return getHardAIMove(board, aiPlayer, humanPlayer);
    } else {
        return getEasyAIMove(board);
    }
}

int AIGame::getHardAIMove(const std::vector<char>& board, char aiPlayer, char humanPlayer)
{
    // Regular minimax for standard game
    std::vector<char> tempBoard = board;
    return minimax(tempBoard, 0, true, aiPlayer, humanPlayer);
}

int AIGame::minimax(std::vector<char>& board, int depth, bool isMaximizing, char aiPlayer, char humanPlayer)
{
    int score = evaluate(board, aiPlayer, humanPlayer);

    if (score == 10 || score == -10) return score;
    if (getAvailableMoves(board).empty()) return 0;

    if (isMaximizing) {
        int best = -1000;
        int bestMove = -1;

        for (int i = 0; i < 9; i++) {
            if (board[i] == ' ') {
                board[i] = aiPlayer;
                int moveScore = minimax(board, depth + 1, false, aiPlayer, humanPlayer);
                board[i] = ' ';

                if (moveScore > best) {
                    best = moveScore;
                    bestMove = i;
                }
            }
        }

        return (depth == 0) ? bestMove : best;
    } else {
        int best = 1000;

        for (int i = 0; i < 9; i++) {
            if (board[i] == ' ') {
                board[i] = humanPlayer;
                int moveScore = minimax(board, depth + 1, true, aiPlayer, humanPlayer);
                board[i] = ' ';

                best = std::min(best, moveScore);
            }
        }

        return best;
    }
}

int AIGame::evaluate(const std::vector<char>& gameBoard, char aiPlayer, char humanPlayer)
{
    // Check rows
    for (int row = 0; row < 3; row++) {
        if (gameBoard[row*3] == gameBoard[row*3+1] && gameBoard[row*3+1] == gameBoard[row*3+2]) {
            if (gameBoard[row*3] == aiPlayer) return 10;
            else if (gameBoard[row*3] == humanPlayer) return -10;
        }
    }

    // Check columns
    for (int col = 0; col < 3; col++) {
        if (gameBoard[col] == gameBoard[col+3] && gameBoard[col+3] == gameBoard[col+6]) {
            if (gameBoard[col] == aiPlayer) return 10;
            else if (gameBoard[col] == humanPlayer) return -10;
        }
    }

    // Check diagonals
    if (gameBoard[0] == gameBoard[4] && gameBoard[4] == gameBoard[8]) {
        if (gameBoard[0] == aiPlayer) return 10;
        else if (gameBoard[0] == humanPlayer) return -10;
    }

    if (gameBoard[2] == gameBoard[4] && gameBoard[4] == gameBoard[6]) {
        if (gameBoard[2] == aiPlayer) return 10;
        else if (gameBoard[2] == humanPlayer) return -10;
    }

    return 0;
}

std::vector<int> AIGame::getAvailableMoves(const std::vector<char>& board)
{
    std::vector<int> moves;
    for (int i = 0; i < 9; i++) {
        if (board[i] == ' ') {
            moves.push_back(i);
        }
    }
    return moves;
}

// Legacy function for compatibility
void startAITicTacToe()
{
    // This function is now handled by GameWindow
    // Implementation moved to MainWindow for better integration
}
