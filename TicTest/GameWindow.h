#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFrame>
#include <QTimer>
#include "classic_game.h"
#include "ai_game.h"

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(const QString &gameMode, QWidget *parent = nullptr);
    ~GameWindow();

    // User management methods
    void setCurrentUser(const QString& username, int userId);
    void setPlayer2(const QString& username, int userId);

private slots:
    void cellClicked();
    void resetGame();
    void backToMenu();

private:
    void setupUI();
    void updateCell(int row, int col);
    void checkGameEnd();
    void makeAIMove();
    void updateStatusLabel();

    QString currentGameMode;
    GameState game;

    // UI Components
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QGridLayout *gridLayout;

    QLabel *titleLabel;
    QLabel *statusLabel;
    QWidget *gridWidget;

    QPushButton *resetBtn;
    QPushButton *backBtn;
    QPushButton *gridButtons[3][3];

    // Game variables
    int aiDifficulty;
    bool isAIGame;

    // User management
    QString currentUsername;
    QString player2Username;
    int currentUserId;
    int player2Id;
    bool gameEnded;
};

#endif
