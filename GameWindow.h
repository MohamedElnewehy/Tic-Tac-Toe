#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QTimer>
#include <QRandomGenerator>
#include <vector>
#include <string>
#include "TicTacToeDB.h"
#include "overwrite_game.h"

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(const QString& gameMode, QWidget *parent = nullptr);
    ~GameWindow();
    void setCurrentUser(const QString& username, int userId);

    // === ✅ Public Test Accessors ===
    bool isGameEnded() const { return gameEnded; }
    char getCurrentPlayer() const { return currentPlayer; }
    char getCellValue(int index) const { return board[index]; }
    bool isAIGameMode() const { return isAIGame; }
    int getAIDifficulty() const { return aiDifficulty; }
    bool makeTestMove(int pos, char player) { return makeMove(pos, player); }
    bool checkTestWin(char player) { return checkWin(player); }

signals:
    void backToMenuRequested();

private slots:
    void onCellClicked();
    void resetGame();
    void backToMenu();
    void onAIMove();

private:
    void setupUI();
    void initializeGame();
    bool makeMove(int position, char player);
    bool checkWin(char player);
    bool checkDraw();
    void endGame(const QString& result);
    void updateStatus(const QString& message);
    void saveGameToDatabase();
    void updateButtonDisplay(int position);
    bool isValidMove(int position, char player);

    // AI methods
    int getAIMove();
    int getEasyAIMove();
    int getMediumAIMove();
    int getHardAIMove();
    int minimax(std::vector<char>& board, int depth, bool isMaximizing, int alpha, int beta);

    int evaluate(const std::vector<char>& board);
    std::vector<int> getAvailableMoves(const std::vector<char>& board);

    // UI Components
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QLabel *statusLabel;
    QPushButton *buttons[9];
    QGridLayout *boardLayout;
    QPushButton *resetBtn;
    QPushButton *backBtn;
    QFrame *boardContainer;
    QWidget *controlWidget;

    // Game state
    std::vector<char> board;
    char currentPlayer;
    bool gameEnded;
    QString gameMode;
    std::vector<std::string> moveHistory;

    // User data
    QString currentUsername;
    int currentUserId;
    TicTacToeDB *database;

    // AI
    QTimer *aiTimer;
    bool isAIGame;
    char aiPlayer;
    char humanPlayer;
    int aiDifficulty; // 1=Easy, 2=Medium, 3=Hard

    // Overwrite game logic
    OverwriteGame *overwriteGame;
};

#endif // GAMEWINDOW_H
