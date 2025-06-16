#ifndef GAME_CORE_H
#define GAME_CORE_H

#include <iostream>
#include <vector>
using namespace std;

typedef struct {
    vector< vector<char> > board;
    char currentPlayer;
    bool gameActive;
} GameState;

// Core functions
void initializeBoard(GameState* game);
void printBoard(const GameState* game);
bool checkWin(const GameState* game);
bool checkDraw(const GameState* game);
void switchPlayer(GameState* game);
bool isValidPosition(int row, int col);
bool makeMove(GameState* game, int row, int col, bool allowOverwrite = false);

#endif

