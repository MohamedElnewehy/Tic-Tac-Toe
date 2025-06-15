#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QTimer>
#include <QFrame>
#include "TicTacToeDB.h"

class HistoryWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryWindow(const QString& username, QWidget *parent = nullptr);
    ~HistoryWindow();

signals:
    void backToMainRequested();

private slots:
    void onGameSelected();
    void onRefreshHistory();
    void onFilterChanged();
    void onRemoveHistoryClicked();
    void onPlayReplay();
    void onPauseReplay();
    void onStopReplay();
    void onNextMove();
    void onPreviousMove();
    void onSliderChanged(int value);
    void onReplayStep();
    void onSpeedChanged(int speed);
    void onBackToMenuClicked();

private:
    void setupUI();
    void loadGameHistory();
    void updateStats();
    void initializeReplay(const std::string& moves);
    void displayMoveAtStep(int step);
    void clearBoard();
    void updateReplayControls();
    void resetReplay();

    // UI Components
    QVBoxLayout *mainLayout;
    QHBoxLayout *filterLayout;
    QHBoxLayout *contentLayout;
    QHBoxLayout *replayControlsLayout;

    QLabel *statsLabel;
    QComboBox *gameModeFilter;
    QPushButton *refreshButton;
    QPushButton *removeHistoryButton;

    QGroupBox *historyGroup;
    QListWidget *gameHistoryList;

    QGroupBox *replayGroup;
    QPushButton *boardButtons[9];
    QGridLayout *boardLayout;

    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *prevButton;
    QPushButton *nextButton;

    QSlider *moveSlider;
    QSlider *speedSlider;
    QLabel *moveLabel;
    QLabel *speedLabel;

    QTimer *replayTimer;

    // Data
    TicTacToeDB *database;
    QString currentUser;
    int currentUserId;
    std::vector<TicTacToeDB::GameRecord> gameHistory;
    std::vector<std::string> currentMoves;
    int currentMoveIndex;
    int replaySpeed;
    bool isReplaying;
    bool isPaused;
};

#endif // HISTORYWINDOW_H
