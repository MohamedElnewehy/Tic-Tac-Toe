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
    QPushButton *gridButtons[3][3];  // تصحيح: إضافة الأبعاد

    // Game variables
    int aiDifficulty;
    bool isAIGame;
};

#endif
