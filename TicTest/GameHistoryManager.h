#ifndef GAMEHISTORYMANAGER_H
#define GAMEHISTORYMANAGER_H

#include <vector>
#include <string>
#include "TicTacToeDB.h"

class GameHistoryManager {
private:
    TicTacToeDB* database;
    std::vector<std::string> currentGameMoves;
    std::string currentGameMode;
    int currentPlayer1Id;
    int currentPlayer2Id;

public:
    GameHistoryManager();
    ~GameHistoryManager();

    void startNewGame(const std::string& gameMode, int player1Id, int player2Id = -1);
    void recordMove(char player, int row, int col);
    void saveGameResult(int winnerId);
    void clearCurrentGame();

    static std::string formatMove(char player, int row, int col);
    static GameHistoryManager* getInstance();
};

#endif
