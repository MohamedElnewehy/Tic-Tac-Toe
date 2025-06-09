#ifndef AI_GAME_H
#define AI_GAME_H

#include "classic_game.h"
#include <random>
#include <algorithm>

enum AILevel {
    EASY = 1,
    MEDIUM = 2,
    HARD = 3
};

struct AIMove {
    int row;
    int col;
    int score;

    AIMove(int r = -1, int c = -1, int s = 0) : row(r), col(c), score(s) {}
};

void startAIGame(int difficulty);
AIMove getAIMove(const GameState* game, AILevel level);
AIMove getEasyMove(const GameState* game);
AIMove getMediumMove(const GameState* game);
AIMove getHardMove(const GameState* game);
int minimax(GameState* game, int depth, bool isMaximizing, char aiPlayer, char humanPlayer);
bool isBoardFull(const GameState* game);
vector<AIMove> getAvailableMoves(const GameState* game);

#endif
