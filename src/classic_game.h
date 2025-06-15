#ifndef CLASSIC_GAME_H
#define CLASSIC_GAME_H

#include <vector>
#include <string>

class ClassicGame
{
public:
    ClassicGame();
    ~ClassicGame();

    // Game state management
    void initializeGame();
    bool makeMove(int position, char player);
    bool isValidMove(int position) const;
    bool checkWin(char player) const;
    bool checkDraw() const;

    // Getters
    char getCurrentPlayer() const { return currentPlayer; }
    char getCellValue(int position) const { return board[position]; }
    bool isGameEnded() const { return gameEnded; }
    std::vector<std::string> getMoveHistory() const { return moveHistory; }
    const std::vector<char>& getBoard() const { return board; }

    // Setters
    void setCurrentPlayer(char player) { currentPlayer = player; }
    void setGameEnded(bool ended) { gameEnded = ended; }

private:
    // Game state
    std::vector<char> board;
    char currentPlayer;
    bool gameEnded;
    std::vector<std::string> moveHistory;

    // Helper methods
    void switchPlayer();
};

// Legacy function for compatibility
void startClassicTicTacToe();

#endif
