#include "ai_engine.h"
#include <iostream>
using namespace std;

bool isMovesLeft(char board[3][3]) {
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
                    best = max(best, minimax(board, depth + 1, false, alpha, beta, aiSymbol, humanSymbol));
                    board[i][j] = ' ';
                    alpha = max(alpha, best);
                    if (beta <= alpha) return best;
                }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < SIZE; ++i)
            for (int j = 0; j < SIZE; ++j)
                if (board[i][j] == ' ') {
                    board[i][j] = humanSymbol;
                    best = min(best, minimax(board, depth + 1, true, alpha, beta, aiSymbol, humanSymbol));
                    board[i][j] = ' ';
                    beta = min(beta, best);
                    if (beta <= alpha) return best;
                }
        return best;
    }
}

Move getRandomMove(char board[3][3]) {
    vector<Move> moves;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == ' ')
                moves.push_back({ i, j });
    return moves[rand() % moves.size()];
}

Move getBestMove(char board[3][3], char aiSymbol, char humanSymbol) {
    int bestVal = -1000;
    Move bestMove = { -1, -1 };
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

void getHumanMove(char board[3][3], char symbol) {
    int r, c;
    while (true) {
        cout << "Enter your move (row col 0–2): ";
        cin >> r >> c;
        if (r >= 0 && r < 3 && c >= 0 && c < 3 && board[r][c] == ' ') {
            board[r][c] = symbol;
            break;
        } else {
            cout << "Invalid move. Try again.\n";
        }
    }
}

void printBoardAI(char board[3][3]) {
    cout << "\n";
    for (int i = 0; i < SIZE; ++i) {
        cout << " ";
        for (int j = 0; j < SIZE; ++j) {
            cout << board[i][j];
            if (j < 2) cout << " | ";
        }
        cout << "\n";
        if (i < 2) cout << "---+---+---\n";
    }
    cout << "\n";
}

void startAIGame() {
    srand(time(0));

    char board[3][3] = {
        { ' ', ' ', ' ' },
        { ' ', ' ', ' ' },
        { ' ', ' ', ' ' }
    };

    Difficulty level;
    int choice;
    cout << "\n=== AI Mode Tic Tac Toe ===\n";
    cout << "Choose AI Level (1-Easy, 2-Medium, 3-Hard): ";
    cin >> choice;
    level = static_cast<Difficulty>(choice - 1);

    char playerSymbol, aiSymbol;
    cout << "Do you want to be X or O? (X goes first): ";
    cin >> playerSymbol;
    playerSymbol = toupper(playerSymbol);
    aiSymbol = (playerSymbol == 'X') ? 'O' : 'X';

    char current = 'X';

    while (true) {
        printBoardAI(board);
        int score = evaluate(board, aiSymbol, playerSymbol);
        if (score == 10) { cout << "AI wins!\n"; break; }
        if (score == -10) { cout << "You win!\n"; break; }
        if (!isMovesLeft(board)) { cout << "It's a draw!\n"; break; }

        if (current == playerSymbol) {
            getHumanMove(board, playerSymbol);
            current = aiSymbol;
        } else {
            cout << "AI is making a move...\n";
            Move m = getAIMove(level, board, aiSymbol, playerSymbol);
            board[m.row][m.col] = aiSymbol;
            current = playerSymbol;
        }
    }

    printBoardAI(board);
}

