#ifndef AI_GAME_H
#define AI_GAME_H

#include <vector>
#include <QRandomGenerator>

class AIGame
{
public:
    AIGame(int difficulty = 1);
    ~AIGame();

    // AI move calculation methods
    int getAIMove(const std::vector<char>& board, char aiPlayer, char humanPlayer);
    int getEasyAIMove(const std::vector<char>& board);
    int getMediumAIMove(const std::vector<char>& board, char aiPlayer, char humanPlayer);
    int getHardAIMove(const std::vector<char>& board, char aiPlayer, char humanPlayer);

    // Minimax algorithm with alpha-beta pruning
    int minimax(std::vector<char>& board, int depth, bool isMaximizing, int alpha, int beta, char aiPlayer, char humanPlayer);
    int evaluate(const std::vector<char>& board, char aiPlayer, char humanPlayer);
    std::vector<int> getAvailableMoves(const std::vector<char>& board);

    // Setters
    void setDifficulty(int difficulty) { aiDifficulty = difficulty; }
    int getDifficulty() const { return aiDifficulty; }

private:
    int aiDifficulty; // 1=Easy, 2=Medium, 3=Hard
};

// Legacy function for compatibility
void startAITicTacToe();

#endif
