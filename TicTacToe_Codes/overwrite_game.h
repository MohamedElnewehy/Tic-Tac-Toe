#ifndef OVERWRITE_GAME_H
#define OVERWRITE_GAME_H

#include "game_core.h"

// Structure to track last moves
struct LastMoves {
    int lastPlayerXRow = -1;
    int lastPlayerXCol = -1;
    int lastPlayerORow = -1;
    int lastPlayerOCol = -1;
};

void startOverwriteGame();
bool canOverwrite(const GameState* game, const LastMoves* lastMoves, int row, int col);

#endif
