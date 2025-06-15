
#ifndef GAMEHISTORYMANAGER_H
#define GAMEHISTORYMANAGER_H

#include <vector>
#include <string>

class GameHistoryManager
{
public:
    GameHistoryManager();
    ~GameHistoryManager();

    void saveGame(int gameId, const std::string& gameData);
    std::vector<std::string> getGameHistory();
    void clearHistory();

private:
    std::vector<std::string> gameHistory;
};

#endif
