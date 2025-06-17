#include <vector>
#include <cstdlib>
#include <algorithm>
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

const int SIZE = 3;

enum Difficulty { EASY, MEDIUM, HARD };

struct Move {
    int row, col;
};

bool isMovesLeft(char board[3][3])
{
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == ' ') return true;
    return false;
}

int evaluate(char board[3][3], char aiSymbol, char humanSymbol) {
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            if (board[i][0] == aiSymbol) return +10;
            else if (board[i][0] == humanSymbol) return -10;
        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            if (board[0][i] == aiSymbol) return +10;
            else if (board[0][i] == humanSymbol) return -10;
        }
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        if (board[0][0] == aiSymbol) return +10;
        else if (board[0][0] == humanSymbol) return -10;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        if (board[0][2] == aiSymbol) return +10;
        else if (board[0][2] == humanSymbol) return -10;
    }
    return 0;
}

int minimax(char board[3][3], int depth, bool isMax, int alpha, int beta, char aiSymbol, char humanSymbol) {
    int score = evaluate(board, aiSymbol, humanSymbol);
    if (score == 10 || score == -10) return score;
    if (!isMovesLeft(board)) return 0;

    if (isMax) {
        int best = -1000;
        for (int i = 0; i < SIZE; ++i)
            for (int j = 0; j < SIZE; ++j)
                if (board[i][j] == ' ') {
                    board[i][j] = aiSymbol;
                    best = std::max(best, minimax(board, depth + 1, false, alpha, beta, aiSymbol, humanSymbol));
                    board[i][j] = ' ';
                    alpha = std::max(alpha, best);
                    if (beta <= alpha) return best;
                }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < SIZE; ++i)
            for (int j = 0; j < SIZE; ++j)
                if (board[i][j] == ' ') {
                    board[i][j] = humanSymbol;
                    best = std::min(best, minimax(board, depth + 1, true, alpha, beta, aiSymbol, humanSymbol));
                    board[i][j] = ' ';
                    beta = std::min(beta, best);
                    if (beta <= alpha) return best;
                }
        return best;
    }
}

Move getRandomMove(char board[3][3]) {
    std::vector<Move> moves;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == ' ')
                moves.push_back({ i, j });
    return moves[rand() % moves.size()];
}

Move getBestMove(char board[3][3], char aiSymbol, char humanSymbol) {
    int bestVal = -1000;
    Move bestMove = {-1, -1};
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == ' ') {
                board[i][j] = aiSymbol;
                int moveVal = minimax(board, 0, false, -1000, 1000, aiSymbol, humanSymbol);
                board[i][j] = ' ';
                if (moveVal > bestVal) {
                    bestMove = { i, j };
                    bestVal = moveVal;
                }
            }
    return bestMove;
}

Move getAIMove(Difficulty level, char board[3][3], char aiSymbol, char humanSymbol) {
    if (level == EASY) return getRandomMove(board);
    if (level == MEDIUM) return (rand() % 2 == 0) ? getRandomMove(board) : getBestMove(board, aiSymbol, humanSymbol);
    return getBestMove(board, aiSymbol, humanSymbol);
}

void clearBoard(char board[SIZE][SIZE]){
    for(int i=0; i<SIZE; i++)
        for(int j=0; j<SIZE; j++)
            board[i][j] = ' ';
}

TEST(AIEngineTests, IsMovesLeftValidation){
    char board[SIZE][SIZE] = {' ',' ',' ',
                              ' ',' ',' ',
                              ' ',' ',' '};
    EXPECT_TRUE(isMovesLeft(board));
    
    //a couple of random moves
    board[0][2] = 'X';
    board[0][1] = 'O';
    board[1][1] = 'X';
    EXPECT_TRUE(isMovesLeft(board));
    
    //fill up the board
    board[0][0] = 'O';
    board[1][0] = 'X';
    board[1][2] = 'O';
    board[2][0] = 'X';
    board[2][1] = 'O';
    board[2][2] = 'X';
    EXPECT_FALSE(isMovesLeft(board));
}

TEST(AIEngineTests, EasyVsMediumDifficulty){
    char board[SIZE][SIZE] = {' ',' ',' ',
                              ' ',' ',' ',
                              ' ',' ',' '};
    Move current_move;
    char ai1sym = 'X';//easy AI
    char ai2sym = 'O';//medium AI
    Difficulty lvlAI1 = EASY;
    Difficulty lvlAI2 = MEDIUM; //here medium should win most of the games
    int medium_wins_counter = 0;//should win more than 50% of the games
    
    for(int games = 0; games < 100; games++){
        while((!evaluate(board,ai2sym,ai1sym))&&(isMovesLeft(board)))
        {
            current_move = getAIMove(lvlAI2,board,ai2sym,ai1sym);//medium AI move
            board[current_move.row][current_move.col] = ai2sym;//updating the board
            if(isMovesLeft(board))
            { 
                current_move = getAIMove(lvlAI1,board,ai1sym,ai2sym);//EASY AI move
                board[current_move.row][current_move.col] = ai1sym;//updating the board
            }
        }
        if(evaluate(board,ai2sym,ai1sym)==10){
            medium_wins_counter += 1;
        }
        clearBoard(board);
    }
    EXPECT_GT(medium_wins_counter,50);
    EXPECT_LT(50,medium_wins_counter);
}

TEST(AIEngineTests, MediumVsHardDifficulty){
    char board[SIZE][SIZE] = {' ',' ',' ',
                              ' ',' ',' ',
                              ' ',' ',' '};
    Move current_move;
    char ai1sym = 'X';//medium AI
    char ai2sym = 'O';//HARD AI
    Difficulty lvlAI1 = MEDIUM;
    Difficulty lvlAI2 = HARD; //here HARD should win most of the games
    int hard_wins_counter = 0;//should win more than 50% of the games
    
    for(int games = 0; games < 100; games++){
        while((!evaluate(board,ai2sym,ai1sym))&&(isMovesLeft(board)))
        {
            current_move = getAIMove(lvlAI2,board,ai2sym,ai1sym);//HARD AI move
            board[current_move.row][current_move.col] = ai2sym;//updating the board
            if(isMovesLeft(board))
            {
                current_move = getAIMove(lvlAI1,board,ai1sym,ai2sym);//MEDIUM AI move
                board[current_move.row][current_move.col] = ai1sym;//updating the board
            }
        }
        if(evaluate(board,ai2sym,ai1sym)==10){
            hard_wins_counter += 1;
        }
        clearBoard(board);
    }
    EXPECT_GT(hard_wins_counter,50);
    EXPECT_LT(50,hard_wins_counter);
}

TEST(AIEngineTests, HardVsHardPerfectPlay){
    char board[SIZE][SIZE] = {' ',' ',' ',
                              ' ',' ',' ',
                              ' ',' ',' '};
    char ai1sym = 'X';//AI1
    char ai2sym = 'O';//AI2
    Difficulty lvlAI1 = HARD;
    Difficulty lvlAI2 = HARD; //here it is expected that neither would win
    Move current_move;
    int hard_wins_counter = 0;//there should be 0 wins
    
    for(int games = 0; games < 100; games++){
        while((!evaluate(board,ai2sym,ai1sym))&&(isMovesLeft(board))){
            current_move = getAIMove(lvlAI1,board,ai1sym,ai2sym);
            board[current_move.row][current_move.col] = ai1sym;
            if(isMovesLeft(board)){
                current_move = getAIMove(lvlAI2,board,ai2sym,ai1sym);
                board[current_move.row][current_move.col] = ai2sym;
            }
        }
        if(evaluate(board,ai2sym,ai1sym))//means either of them won
            hard_wins_counter += 1;
        clearBoard(board);
    }
    EXPECT_EQ(hard_wins_counter,0);
}


