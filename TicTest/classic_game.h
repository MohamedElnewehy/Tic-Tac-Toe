#ifndef CLASSIC_GAME_H
#define CLASSIC_GAME_H

#include <iostream>
#include <vector>

using namespace std;

struct GameState {
    vector<vector<char>> board;
    char currentPlayer;
    bool gameActive;

    GameState() : board(3, vector<char>(3, ' ')), currentPlayer('X'), gameActive(true) {}
};

void initializeBoard(GameState* game);
void printBoard(const GameState* game);
bool makeMove(GameState* game, int row, int col, bool allowOverwrite = false);
bool checkWin(const GameState* game);
bool checkDraw(const GameState* game);
void switchPlayer(GameState* game);
void startClassicTicTacToe();

#endif
