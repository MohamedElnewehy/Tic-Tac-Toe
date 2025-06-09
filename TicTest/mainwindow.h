#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QFrame>
#include <QStackedWidget>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QMessageBox>
#include <QInputDialog>

// Include game modules
#include "classic_game.h"
#include "overwrite_game.h"
#include "ai_game.h"
#include "GameWindow.h"
#include "HistoryWindow.h"
#include "TicTacToeDB.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showMainMenu();
    void startClassicGame();
    void startOverwriteGame();
    void showAIMenu();
    void startEasyAI();
    void startMediumAI();
    void startHardAI();
    void showGameHistory();
    void showLogin();

private:
    void setupUI();
    void setupMainMenu();
    void setupAIMenu();
    void setupLoginMenu();
    void applyModernStyling();
    QPushButton* createStyledButton(const QString &text, const QString &color);
    void addButtonAnimation(QPushButton *button);
    bool authenticateUser();

    QStackedWidget *stackedWidget;
    QWidget *mainMenuWidget;
    QWidget *aiMenuWidget;
    QWidget *loginWidget;

    // Login components
    QVBoxLayout *loginLayout;
    QLabel *loginTitleLabel;
    QPushButton *loginBtn;
    QPushButton *registerBtn;
    QPushButton *guestBtn;

    // Main Menu Components
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLabel *userLabel;
    QFrame *headerFrame;
    QFrame *buttonFrame;

    QPushButton *classicGameBtn;
    QPushButton *overwriteGameBtn;
    QPushButton *aiGameBtn;
    QPushButton *historyBtn;
    QPushButton *logoutBtn;
    QPushButton *exitBtn;

    // AI Menu Components
    QVBoxLayout *aiLayout;
    QLabel *aiTitleLabel;
    QPushButton *easyAIBtn;
    QPushButton *mediumAIBtn;
    QPushButton *hardAIBtn;
    QPushButton *backToMainBtn;

    // User management
    TicTacToeDB *database;
    QString currentUsername;
    int currentUserId;
    bool isLoggedIn;
};

#endif
