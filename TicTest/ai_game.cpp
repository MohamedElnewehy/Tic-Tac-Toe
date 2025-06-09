#include "ai_game.h"

void startAIGame(int difficulty) {
    GameState game;
    initializeBoard(&game);

    AILevel level = static_cast<AILevel>(difficulty);
    string levelName = (level == EASY) ? "Easy" :
                           (level == MEDIUM) ? "Medium" : "Hard";

    cout << "\n=== AI Game - " << levelName << " Level ===\n";
    cout << "You are X, AI is O\n";
    printBoard(&game);

    while (game.gameActive) {
        if (game.currentPlayer == 'X') {
            // Human player turn
            int row, col;
            cout << "Your turn! Enter row and column (0-2): ";
            cin >> row >> col;

            if (!makeMove(&game, row, col, false)) continue;
        } else {
            // AI turn
            cout << "AI is thinking...\n";
            AIMove aiMove = getAIMove(&game, level);

            if (aiMove.row != -1 && aiMove.col != -1) {
                makeMove(&game, aiMove.row, aiMove.col, false);
                cout << "AI plays: " << aiMove.row << ", " << aiMove.col << "\n";
            }
        }

        printBoard(&game);

        if (checkWin(&game)) {
            if (game.currentPlayer == 'X') {
                cout << "Congratulations! You won!\n";
            } else {
                cout << "AI wins! Better luck next time!\n";
            }
            game.gameActive = false;
            break;
        }

        if (checkDraw(&game)) {
            cout << "Game ended in a draw!\n";
            game.gameActive = false;
            break;
        }

        switchPlayer(&game);
    }

    cout << "Press Enter to return to main menu...";
    cin.ignore();
    cin.get();
}

AIMove getAIMove(const GameState* game, AILevel level) {
    switch (level) {
    case EASY:
        return getEasyMove(game);
    case MEDIUM:
        return getMediumMove(game);
    case HARD:
        return getHardMove(game);
    default:
        return getEasyMove(game);
    }
}

AIMove getEasyMove(const GameState* game) {
    vector<AIMove> availableMoves = getAvailableMoves(game);

    if (availableMoves.empty()) {
        return AIMove(-1, -1, 0);
    }

    // Random move for easy level
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, availableMoves.size() - 1);

    return availableMoves[dis(gen)];
}

AIMove getMediumMove(const GameState* game) {
    vector<AIMove> availableMoves = getAvailableMoves(game);

    if (availableMoves.empty()) {
        return AIMove(-1, -1, 0);
    }

    // 70% chance to make optimal move, 30% random
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 10);

    if (dis(gen) <= 7) {
        return getHardMove(game);
    } else {
        return getEasyMove(game);
    }
}

AIMove getHardMove(const GameState* game) {
    vector<AIMove> availableMoves = getAvailableMoves(game);

    if (availableMoves.empty()) {
        return AIMove(-1, -1, 0);
    }

    AIMove bestMove(-1, -1, -1000);

    for (const auto& move : availableMoves) {
        GameState tempGame = *game;
        tempGame.board[move.row][move.col] = 'O';

        int score = minimax(&tempGame, 0, false, 'O', 'X');

        if (score > bestMove.score) {
            bestMove = AIMove(move.row, move.col, score);
        }
    }

    return bestMove;
}

int minimax(GameState* game, int depth, bool isMaximizing, char aiPlayer, char humanPlayer) {
    // Check terminal states
    game->currentPlayer = aiPlayer;
    if (checkWin(game)) return 10 - depth;

    game->currentPlayer = humanPlayer;
    if (checkWin(game)) return depth - 10;

    if (isBoardFull(game)) return 0;

    if (isMaximizing) {
        int maxEval = -1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (game->board[i][j] == ' ') {
                    game->board[i][j] = aiPlayer;
                    int eval = minimax(game, depth + 1, false, aiPlayer, humanPlayer);
                    game->board[i][j] = ' ';
                    maxEval = max(maxEval, eval);
                }
            }
        }
        return maxEval;
    } else {
        int minEval = 1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (game->board[i][j] == ' ') {
                    game->board[i][j] = humanPlayer;
                    int eval = minimax(game, depth + 1, true, aiPlayer, humanPlayer);
                    game->board[i][j] = ' ';
                    minEval = min(minEval, eval);
                }
            }
        }
        return minEval;
    }
}

bool isBoardFull(const GameState* game) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game->board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

vector<AIMove> getAvailableMoves(const GameState* game) {
    vector<AIMove> moves;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game->board[i][j] == ' ') {
                moves.push_back(AIMove(i, j, 0));
            }
        }
    }
    return moves;
}
