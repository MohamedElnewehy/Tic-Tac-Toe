#ifndef OVERWRITE_GAME_H
#define OVERWRITE_GAME_H

#include <vector>
#include <string>

class OverwriteGame
{
public:
    OverwriteGame();
    ~OverwriteGame();

    // Game state management
    void initializeGame();
    bool makeMove(int position, char player);
    bool isValidMove(int position, char player) const;
    bool checkWin(char player) const;
    bool checkDraw() const;

    // Getters
    char getCurrentPlayer() const { return currentPlayer; }
    char getCellValue(int position) const { return board[position]; }
    int getTurnCount() const { return turnCount; }
    bool isGameEnded() const { return gameEnded; }
    std::vector<std::string> getMoveHistory() const { return moveHistory; }

    // Setters
    void setCurrentPlayer(char player) { currentPlayer = player; }
    void setGameEnded(bool ended) { gameEnded = ended; }

private:
    // Game state
    std::vector<char> board;
    char currentPlayer;
    bool gameEnded;
    std::vector<std::string> moveHistory;

    // Overwrite Mode specific tracking
    int turnCount;
    std::vector<int> cellTurn;
    std::vector<char> cellOwner;

    // Helper methods
    bool canOverwrite(int position, char player) const;
    void switchPlayer();
};

#endif // OVERWRITE_GAME_H
