
#include "GameHistoryManager.h"

GameHistoryManager::GameHistoryManager()
{
    // Constructor implementation
}

GameHistoryManager::~GameHistoryManager()
{
    // Destructor implementation
}

void GameHistoryManager::saveGame(int gameId, const std::string& gameData)
{
    gameHistory.push_back(gameData);
}

std::vector<std::string> GameHistoryManager::getGameHistory()
{
    return gameHistory;
}

void GameHistoryManager::clearHistory()
{
    gameHistory.clear();
}
