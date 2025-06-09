#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QComboBox>
#include <QSlider>
#include <QTimer>
#include <QMessageBox>
#include <QFrame>
#include "TicTacToeDB.h"

class HistoryWindow : public QDialog
{
    Q_OBJECT

public:
    HistoryWindow(const QString& username, QWidget *parent = nullptr);
    ~HistoryWindow();

private slots:
    void onGameSelected();
    void onRefreshHistory();
    void onFilterChanged();
    void onPlayReplay();
    void onPauseReplay();
    void onStopReplay();
    void onNextMove();
    void onPreviousMove();
    void onSliderChanged(int value);
    void onReplayStep();
    void onSpeedChanged(int speed);
    void onRemoveHistoryClicked();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *contentLayout;
    QHBoxLayout *filterLayout;

    QComboBox *gameModeFilter;
    QGroupBox *historyGroup;
    QListWidget *gameHistoryList;
    QPushButton *refreshButton;
    QPushButton *removeHistoryButton;

    QGridLayout *boardLayout;
    QPushButton *boardButtons[9];

    // Replay Controls
    QGroupBox *replayGroup;
    QHBoxLayout *replayControlsLayout;
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QSlider *moveSlider;
    QLabel *moveLabel;
    QSlider *speedSlider;
    QLabel *speedLabel;

    QLabel *statsLabel;

    TicTacToeDB *database;
    QString currentUser;
    int currentUserId;
    std::vector<TicTacToeDB::GameRecord> gameHistory;

    // Replay System
    QTimer *replayTimer;
    std::vector<std::string> currentMoves;
    int currentMoveIndex;
    int replaySpeed;
    bool isReplaying;
    bool isPaused;

    void setupUI();
    void loadGameHistory();
    void clearBoard();
    void updateStats();

    // Replay functions
    void initializeReplay(const std::string& moves);
    void displayMoveAtStep(int step);
    void updateReplayControls();
    void resetReplay();
};

#endif
