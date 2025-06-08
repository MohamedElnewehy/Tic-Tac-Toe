#include "AI.h"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include "gtest/gtest.h"
#include <iostream>

using namespace std;
const int SIZE = 3;
/*this is a helper function not in need of testing*/
bool isMovesLeft(char board[3][3])
{
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == ' ') return true;
    return false;
}
/*this is also a helper function, not in need of testing*/
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
/*another helper function*/
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
/*another helper function for the AI, will be tested through the difficulty*/
Move getRandomMove(char board[3][3]) {
    std::vector<Move> moves;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == ' ')
                moves.push_back({ i, j });
    return moves[rand() % moves.size()];
}
/*the helper function for the hard AI difficulty*/
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
/*this function is the one that will test all 3 difficulties of the AI model*/
Move getAIMove(Difficulty level, char board[3][3], char aiSymbol, char humanSymbol) {
    if (level == EASY) return getRandomMove(board);
    if (level == MEDIUM) return (rand() % 2 == 0) ? getRandomMove(board) : getBestMove(board, aiSymbol, humanSymbol);
    return getBestMove(board, aiSymbol, humanSymbol);
}
//test part
/*made a test for each symbol to make sure it is general for all symbols
 *hard part doesnt make a difference since it will be tested through draws */

/*a test helper function to reset the board*/
void clearboard(char board[SIZE][SIZE]){
for(int i=0;i<SIZE;i++)
	for(int j=0;j<SIZE;j++)
	board[i][j] = ' ';
}
/*end of the test helper functions*/
TEST(AI,EASY_MEDIUM)//here it tests the fact that medium is harder than easy
{
char board[SIZE][SIZE] = {' ',' ',' ',
			  ' ',' ',' ',
			  ' ',' ',' '};
Move current_move;
char ai1sym = 'X';//easy AI
char ai2sym = 'O';//medium AI
Difficulty lvlAI1 = EASY;
Difficulty lvlAI2 = MEDIUM; //here medium should win most of the games
int medium_wins_counter = 0;//should win more than 50% of the games
for(int games = 0; games <100; games++){

while((!evaluate(board,ai2sym,ai1sym))&&(isMovesLeft(board)))
{
current_move = getAIMove(lvlAI2,board,ai2sym,ai1sym);//medium AI move
board[current_move.row][current_move.col] =ai2sym;//updating the board
if(isMovesLeft(board))
{ 
current_move = getAIMove(lvlAI1,board,ai1sym,ai2sym);//EASY AI move
board[current_move.row][current_move.col] =ai1sym;//updating the board}
}}
if(evaluate(board,ai2sym,ai1sym)==10){
	medium_wins_counter += 1;}
clearboard(board);

}
EXPECT_GT(medium_wins_counter,50);
EXPECT_LT(50,medium_wins_counter);
}
TEST(AI,MEDIUM_HARD)//here it tests the fact that hard is harder than medium
{
char board[SIZE][SIZE] = {' ',' ',' ',
                          ' ',' ',' ',
                          ' ',' ',' '};
Move current_move;
char ai1sym = 'X';//medium AI
char ai2sym = 'O';//HARD AI
Difficulty lvlAI1 = MEDIUM;
Difficulty lvlAI2 = HARD; //here HARD should win most of the games
int hard_wins_counter = 0;//should win more than 50% of the games
for(int games = 0; games <100; games++){

while((!evaluate(board,ai2sym,ai1sym))&&(isMovesLeft(board)))
{
current_move = getAIMove(lvlAI2,board,ai2sym,ai1sym);//medium AI move
board[current_move.row][current_move.col] =ai2sym;//updating the board
if(isMovesLeft(board))
{
current_move = getAIMove(lvlAI1,board,ai1sym,ai2sym);//EASY AI move
board[current_move.row][current_move.col] =ai1sym;//updating the board}
}}
if(evaluate(board,ai2sym,ai1sym)==10){
        hard_wins_counter += 1;}
clearboard(board);

}
EXPECT_GT(hard_wins_counter,50);
EXPECT_LT(50,hard_wins_counter);
}

TEST(AI,HARD)//here I am ensuring that HARD always implements the perfect move
{
char board[SIZE][SIZE] = {' ',' ',' ',
                          ' ',' ',' ',
                          ' ',' ',' '};
char ai1sym = 'X';//AI1
char ai2sym = 'O';//AI2
Difficulty lvlAI1 = HARD;
Difficulty lvlAI2 = HARD; //here it is expected that neither would win
Move current_move;
int hard_wins_counter = 0;//there should be 0 wins
for(int games = 0; games <100; games++){
while((!evaluate(board,ai2sym,ai1sym))&&(isMovesLeft(board))){
current_move = getAIMove(lvlAI1,board,ai1sym,ai2sym);
board[current_move.row][current_move.col] =ai1sym;
if(isMovesLeft(board)){
current_move = getAIMove(lvlAI2,board,ai2sym,ai1sym);
board[current_move.row][current_move.col] =ai2sym;
}}
if(evaluate(board,ai2sym,ai1sym))//means either of them won
        hard_wins_counter += 1;
clearboard(board);

}
EXPECT_EQ(hard_wins_counter,0);

}
//end of the test part
int main(int argc, char** argv){
testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}
