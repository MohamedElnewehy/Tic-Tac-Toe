#ifndef AI_H
#define AI_H

enum Difficulty { EASY, MEDIUM, HARD };

struct Move {
    int row, col;
};

Move getAIMove(Difficulty level, char board[3][3], char aiSymbol, char humanSymbol);
bool isMovesLeft(char board[3][3]);
int evaluate(char board[3][3], char aiSymbol, char humanSymbol);

#endif
