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

// Include game modules
#include "classic_game.h"
#include "overwrite_game.h"
#include "ai_game.h"
#include "GameWindow.h"

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
    void showSettings();

private:
    void setupUI();
    void setupMainMenu();
    void setupAIMenu();
    void applyModernStyling();
    QPushButton* createStyledButton(const QString &text, const QString &color);
    void addButtonAnimation(QPushButton *button);

    QStackedWidget *stackedWidget;
    QWidget *mainMenuWidget;
    QWidget *aiMenuWidget;

    // Main Menu Components
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QFrame *headerFrame;
    QFrame *buttonFrame;

    QPushButton *classicGameBtn;
    QPushButton *overwriteGameBtn;
    QPushButton *aiGameBtn;
    QPushButton *historyBtn;
    QPushButton *settingsBtn;
    QPushButton *exitBtn;

    // AI Menu Components
    QVBoxLayout *aiLayout;
    QLabel *aiTitleLabel;
    QPushButton *easyAIBtn;
    QPushButton *mediumAIBtn;
    QPushButton *hardAIBtn;
    QPushButton *backToMainBtn;
};

#endif
