#include "GameHistoryManager.h"
#include <iostream>

GameHistoryManager::GameHistoryManager() : database(nullptr), currentPlayer1Id(-1), currentPlayer2Id(-1) {
    try {
        database = new TicTacToeDB();
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize database: " << e.what() << std::endl;
    }
}

GameHistoryManager::~GameHistoryManager() {
    delete database;
}

void GameHistoryManager::startNewGame(const std::string& gameMode, int player1Id, int player2Id) {
    currentGameMoves.clear();
    currentGameMode = gameMode;
    currentPlayer1Id = player1Id;
    currentPlayer2Id = player2Id;
}

void GameHistoryManager::recordMove(char player, int row, int col) {
    std::string move = formatMove(player, row, col);
    currentGameMoves.push_back(move);
}

void GameHistoryManager::saveGameResult(int winnerId) {
    if (!database || currentPlayer1Id == -1) {
        return;
    }

    try {
        database->saveGame(currentPlayer1Id, currentPlayer2Id, winnerId, currentGameMoves, currentGameMode);
    } catch (const std::exception& e) {
        std::cerr << "Failed to save game: " << e.what() << std::endl;
    }
}

void GameHistoryManager::clearCurrentGame() {
    currentGameMoves.clear();
    currentGameMode = "";
    currentPlayer1Id = -1;
    currentPlayer2Id = -1;
}

std::string GameHistoryManager::formatMove(char player, int row, int col) {
    return std::string(1, player) + std::to_string(row) + std::to_string(col);
}

GameHistoryManager* GameHistoryManager::getInstance() {
    static GameHistoryManager instance;
    return &instance;
}
