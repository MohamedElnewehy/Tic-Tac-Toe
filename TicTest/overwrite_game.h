#ifndef OVERWRITE_GAME_H
#define OVERWRITE_GAME_H

#include "classic_game.h"

struct LastMoves {
    int lastPlayerXRow = -1;
    int lastPlayerXCol = -1;
    int lastPlayerORow = -1;
    int lastPlayerOCol = -1;
};

void startOverwriteGame();
bool canOverwrite(const GameState* game, const LastMoves* lastMoves, int row, int col);

#endif
