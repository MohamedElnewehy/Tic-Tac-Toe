#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>

const int SIZE = 3;

enum Difficulty { EASY, MEDIUM, HARD };

struct Move {
    int row, col;
};

bool isMovesLeft(char board[3][3]);
int evaluate(char board[3][3], char aiSymbol, char humanSymbol);
Move getAIMove(Difficulty level, char board[3][3], char aiSymbol, char humanSymbol);
void getHumanMove(char board[3][3], char symbol);
void printBoardAI(char board[3][3]);
void startAIGame();

#endif

