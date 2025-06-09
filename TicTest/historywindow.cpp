#include "HistoryWindow.h"
#include <QMessageBox>
#include <QSplitter>
#include <QFrame>

HistoryWindow::HistoryWindow(const QString& username, QWidget *parent)
    : QDialog(parent), currentUser(username), currentUserId(-1),
    currentMoveIndex(0), replaySpeed(1000), isReplaying(false), isPaused(false)
{
    setWindowTitle("Game History - " + username);
    setMinimumSize(1200, 800);

    try {
        database = new TicTacToeDB();
        currentUserId = database->getUserId(username.toStdString());

        if (currentUserId == -1) {
            QMessageBox::warning(this, "Error", "User not found!");
            return;
        }

        replayTimer = new QTimer(this);
        connect(replayTimer, &QTimer::timeout, this, &HistoryWindow::onReplayStep);

        setupUI();
        loadGameHistory();
        updateStats();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Database Error", e.what());
    }
}

HistoryWindow::~HistoryWindow()
{
    delete database;
}

void HistoryWindow::setupUI()
{
    // Set main window background to match GameWindow
    setStyleSheet(
        "QDialog {"
        "background-color: #f8f9fa;"
        "}"
        );

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(25, 20, 25, 25);

    // Title Section with GameWindow styling
    QLabel *titleLabel = new QLabel("Game History - " + currentUser);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(50);
    titleLabel->setStyleSheet(
        "QLabel {"
        "background-color: #34495e;"
        "color: white;"
        "font-size: 20px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "border-radius: 10px;"
        "padding: 10px;"
        "border: 2px solid #2c3e50;"
        "}"
        );

    // Filter and refresh section
    filterLayout = new QHBoxLayout();

    gameModeFilter = new QComboBox();
    gameModeFilter->setFixedHeight(40); // Increase height for better appearance
    gameModeFilter->addItem("All Games");
    gameModeFilter->addItem("Classic");
    gameModeFilter->addItem("Overwrite");
    gameModeFilter->addItem("AI Easy");
    gameModeFilter->addItem("AI Medium");
    gameModeFilter->addItem("AI Hard");

    // Enhanced modern styling
    gameModeFilter->setStyleSheet(
        "QComboBox {"
        "background-color: #ffffff;"
        "border: 2px solid #dee2e6;"
        "border-radius: 8px;"
        "padding: 8px 12px;"
        "font-size: 14px;"
        "font-weight: 500;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "color: #495057;"
        "min-width: 120px;"
        "}"
        "QComboBox:hover {"
        "border-color: #4A90E2;"
        "background-color: #f8f9fa;"
        "}"
        "QComboBox:focus {"
        "border-color: #4A90E2;"
        "outline: none;"
        "}"
        "QComboBox::drop-down {"
        "subcontrol-origin: padding;"
        "subcontrol-position: top right;"
        "width: 30px;"
        "border-left: 1px solid #dee2e6;"
        "border-top-right-radius: 8px;"
        "border-bottom-right-radius: 8px;"
        "background-color: #f8f9fa;"
        "}"
        "QComboBox::drop-down:hover {"
        "background-color: #4A90E2;"
        "}"
        "QComboBox::down-arrow {"
        "image: none;"
        "border-left: 5px solid transparent;"
        "border-right: 5px solid transparent;"
        "border-top: 6px solid #6c757d;"
        "margin-top: 2px;"
        "}"
        "QComboBox::down-arrow:hover {"
        "border-top-color: white;"
        "}"
        "QComboBox QAbstractItemView {"
        "border: 2px solid #4A90E2;"
        "border-radius: 8px;"
        "background-color: white;"
        "selection-background-color: #4A90E2;"
        "selection-color: white;"
        "outline: none;"
        "padding: 4px;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "padding: 8px 12px;"
        "border-radius: 4px;"
        "margin: 1px;"
        "font-size: 14px;"
        "color: #495057;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "background-color: #e3f2fd;"
        "color: #4A90E2;"
        "}"
        "QComboBox QAbstractItemView::item:selected {"
        "background-color: #4A90E2;"
        "color: white;"
        "}"
        );

    connect(gameModeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HistoryWindow::onFilterChanged);

    refreshButton = new QPushButton("🔄 Refresh");
    refreshButton->setFixedSize(120, 35);
    refreshButton->setStyleSheet(
        "QPushButton {"
        "background-color: #007bff;"
        "color: white;"
        "border: none;"
        "border-radius: 8px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "background-color: #0056b3;"
        "transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "background-color: #004085;"
        "transform: translateY(0px);"
        "}"
        );
    connect(refreshButton, &QPushButton::clicked, this, &HistoryWindow::onRefreshHistory);

    // Remove history button
    removeHistoryButton = new QPushButton("🗑️ Remove All History");
    removeHistoryButton->setFixedSize(180, 35);
    removeHistoryButton->setStyleSheet(
        "QPushButton {"
        "background-color: #dc3545;"
        "color: white;"
        "border: none;"
        "border-radius: 8px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "background-color: #c82333;"
        "transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "background-color: #bd2130;"
        "transform: translateY(0px);"
        "}"
        );
    connect(removeHistoryButton, &QPushButton::clicked, this, &HistoryWindow::onRemoveHistoryClicked);

    filterLayout->addWidget(new QLabel("Filter by Mode:"));
    filterLayout->addWidget(gameModeFilter);
    filterLayout->addStretch();
    filterLayout->addWidget(refreshButton);
    filterLayout->addWidget(removeHistoryButton);

    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(filterLayout);

    // Stats section with GameWindow styling
    statsLabel = new QLabel();
    statsLabel->setAlignment(Qt::AlignCenter);
    statsLabel->setFixedHeight(32);
    statsLabel->setStyleSheet(
        "QLabel {"
        "background-color: #3498db;"
        "color: white;"
        "font-size: 15px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "border-radius: 8px;"
        "padding: 6px;"
        "border: 2px solid #2980b9;"
        "}"
        );
    mainLayout->addWidget(statsLabel);
    mainLayout->addSpacing(8);

    contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15);

    // Left side - Game list with GameWindow container styling
    QFrame *historyContainer = new QFrame();
    historyContainer->setFixedWidth(400);
    historyContainer->setStyleSheet(
        "QFrame {"
        "background-color: #f8f9fa;"
        "border-radius: 15px;"
        "border: 3px solid #dee2e6;"
        "padding: 10px;"
        "}"
        );

    QVBoxLayout *historyContainerLayout = new QVBoxLayout(historyContainer);

    historyGroup = new QGroupBox("📋 Games");
    historyGroup->setStyleSheet(
        "QGroupBox {"
        "font-weight: bold;"
        "font-size: 16px;"
        "color: #2c3e50;"
        "border: 2px solid #3498db;"
        "border-radius: 8px;"
        "margin-top: 10px;"
        "padding-top: 10px;"
        "background-color: white;"
        "}"
        "QGroupBox::title {"
        "subcontrol-origin: margin;"
        "left: 10px;"
        "padding: 0 8px 0 8px;"
        "background-color: white;"
        "}"
        );

    QVBoxLayout *historyLayout = new QVBoxLayout(historyGroup);

    gameHistoryList = new QListWidget();
    gameHistoryList->setSelectionMode(QAbstractItemView::SingleSelection);
    gameHistoryList->setSelectionBehavior(QAbstractItemView::SelectRows);
    gameHistoryList->setFocusPolicy(Qt::StrongFocus);
    gameHistoryList->setStyleSheet(
        "QListWidget {"
        "border: 1px solid #ced4da;"
        "border-radius: 8px;"
        "background-color: white;"
        "outline: none;"
        "font-size: 14px;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QListWidget::item {"
        "padding: 8px;"
        "border-bottom: 1px solid #e9ecef;"
        "color: #333333;"
        "}"
        "QListWidget::item:hover {"
        "background-color: rgba(74, 144, 226, 0.1);"
        "color: #4A90E2;"
        "}"
        "QListWidget::item:selected {"
        "background-color: #4A90E2;"
        "color: white;"
        "font-weight: bold;"
        "}"
        "QListWidget::item:selected:!active {"
        "background-color: #4A90E2;"
        "color: white;"
        "}"
        );
    connect(gameHistoryList, &QListWidget::itemClicked, this, &HistoryWindow::onGameSelected);
    historyLayout->addWidget(gameHistoryList);

    historyContainerLayout->addWidget(historyGroup);

    // Right side - Board only (no details text box)
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // Board title
    QLabel *boardTitle = new QLabel("🎮 Game Board");
    boardTitle->setAlignment(Qt::AlignCenter);
    boardTitle->setStyleSheet(
        "QLabel {"
        "font-weight: bold;"
        "font-size: 16px;"
        "color: #2c3e50;"
        "padding: 10px;"
        "background-color: white;"
        "border-radius: 8px;"
        "margin-bottom: 10px;"
        "border: 2px solid #3498db;"
        "}"
        );

    // Board with EXACT GameWindow container and button styling
    QFrame *boardContainer = new QFrame();
    boardContainer->setFixedSize(350, 350);
    boardContainer->setStyleSheet(
        "QFrame {"
        "background-color: #f8f9fa;"
        "border-radius: 15px;"
        "border: 3px solid #dee2e6;"
        "padding: 10px;"
        "}"
        );

    QWidget *boardWidget = new QWidget(boardContainer);
    boardWidget->setGeometry(15, 15, 320, 320);
    boardLayout = new QGridLayout(boardWidget);
    boardLayout->setSpacing(5);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 9; i++) {
        boardButtons[i] = new QPushButton();
        boardButtons[i]->setFixedSize(102, 102);
        boardButtons[i]->setEnabled(false);
        boardButtons[i]->setStyleSheet(
            "QPushButton {"
            "background-color: #ffffff;"
            "border: 2px solid #4A90E2;"
            "border-radius: 10px;"
            "font-size: 42px;"
            "font-weight: bold;"
            "font-family: 'Arial', sans-serif;"
            "color: #495057;"
            "}"
            );
        boardLayout->addWidget(boardButtons[i], i/3, i%3);
    }

    rightLayout->addWidget(boardTitle);
    rightLayout->addWidget(boardContainer, 0, Qt::AlignCenter);

    // Replay Controls
    replayGroup = new QGroupBox("🎬 Replay Controls");
    replayGroup->setStyleSheet(
        "QGroupBox {"
        "font-weight: bold;"
        "font-size: 16px;"
        "color: #2c3e50;"
        "border: 2px solid #3498db;"
        "border-radius: 8px;"
        "margin-top: 10px;"
        "padding-top: 10px;"
        "background-color: white;"
        "}"
        "QGroupBox::title {"
        "subcontrol-origin: margin;"
        "left: 10px;"
        "padding: 0 8px 0 8px;"
        "background-color: white;"
        "}"
        );

    QVBoxLayout *replayLayout = new QVBoxLayout(replayGroup);

    // Control buttons
    replayControlsLayout = new QHBoxLayout();

    QString buttonStyle =
        "QPushButton {"
        "background-color: #007bff;"
        "color: white;"
        "border: none;"
        "border-radius: 8px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "padding: 8px 16px;"
        "min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "background-color: #0056b3;"
        "transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "background-color: #004085;"
        "transform: translateY(0px);"
        "}"
        "QPushButton:disabled {"
        "background-color: #6c757d;"
        "color: #adb5bd;"
        "}";

    playButton = new QPushButton("▶️ Play");
    playButton->setStyleSheet(buttonStyle);

    pauseButton = new QPushButton("⏸️ Pause");
    pauseButton->setStyleSheet(buttonStyle);
    pauseButton->setEnabled(false);

    stopButton = new QPushButton("⏹️ Stop");
    stopButton->setStyleSheet(buttonStyle);
    stopButton->setEnabled(false);

    prevButton = new QPushButton("⏮️ Prev");
    prevButton->setStyleSheet(buttonStyle);

    nextButton = new QPushButton("⏭️ Next");
    nextButton->setStyleSheet(buttonStyle);

    connect(playButton, &QPushButton::clicked, this, &HistoryWindow::onPlayReplay);
    connect(pauseButton, &QPushButton::clicked, this, &HistoryWindow::onPauseReplay);
    connect(stopButton, &QPushButton::clicked, this, &HistoryWindow::onStopReplay);
    connect(prevButton, &QPushButton::clicked, this, &HistoryWindow::onPreviousMove);
    connect(nextButton, &QPushButton::clicked, this, &HistoryWindow::onNextMove);

    replayControlsLayout->addWidget(prevButton);
    replayControlsLayout->addWidget(playButton);
    replayControlsLayout->addWidget(pauseButton);
    replayControlsLayout->addWidget(stopButton);
    replayControlsLayout->addWidget(nextButton);

    // Move slider
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    moveLabel = new QLabel("Move: 0/0");
    moveLabel->setStyleSheet("font-weight: bold; color: #2c3e50; font-family: 'Segoe UI', Arial, sans-serif;");

    moveSlider = new QSlider(Qt::Horizontal);
    moveSlider->setMinimum(0);
    moveSlider->setMaximum(0);
    moveSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "border: 1px solid #bbb;"
        "background: white;"
        "height: 10px;"
        "border-radius: 4px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "background: #4A90E2;"
        "border: 1px solid #777;"
        "height: 10px;"
        "border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "background: #4A90E2;"
        "border: 1px solid #5c5c5c;"
        "width: 18px;"
        "margin: -2px 0;"
        "border-radius: 9px;"
        "}"
        );
    connect(moveSlider, &QSlider::valueChanged, this, &HistoryWindow::onSliderChanged);

    sliderLayout->addWidget(new QLabel("🎬 Timeline:"));
    sliderLayout->addWidget(moveSlider);
    sliderLayout->addWidget(moveLabel);

    // Speed control
    QHBoxLayout *speedLayout = new QHBoxLayout();
    speedLabel = new QLabel("Speed: 2x");
    speedLabel->setStyleSheet("font-weight: bold; color: #2c3e50; font-family: 'Segoe UI', Arial, sans-serif;");

    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setMinimum(1);
    speedSlider->setMaximum(5);
    speedSlider->setValue(2);
    speedSlider->setFixedWidth(150);
    speedSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "border: 1px solid #bbb;"
        "background: white;"
        "height: 10px;"
        "border-radius: 4px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "background: #28a745;"
        "border: 1px solid #777;"
        "height: 10px;"
        "border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "background: #28a745;"
        "border: 1px solid #5c5c5c;"
        "width: 18px;"
        "margin: -2px 0;"
        "border-radius: 9px;"
        "}"
        );
    connect(speedSlider, &QSlider::valueChanged, this, &HistoryWindow::onSpeedChanged);

    speedLayout->addWidget(new QLabel("⚡ Speed:"));
    speedLayout->addWidget(speedSlider);
    speedLayout->addWidget(speedLabel);
    speedLayout->addStretch();

    replayLayout->addLayout(replayControlsLayout);
    replayLayout->addLayout(sliderLayout);
    replayLayout->addLayout(speedLayout);

    rightLayout->addWidget(replayGroup);

    contentLayout->addWidget(historyContainer);
    contentLayout->addLayout(rightLayout);

    mainLayout->addLayout(contentLayout);
    mainLayout->addSpacing(12);
}

void HistoryWindow::onRemoveHistoryClicked()
{
    // Create custom message box
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("🗑️ Confirm Deletion");
    msgBox.setText("Are you sure you want to delete all game history?");
    msgBox.setInformativeText("This action will permanently remove all your game records and cannot be undone!");
    msgBox.setIcon(QMessageBox::Warning);

    // Add custom buttons with better text
    QPushButton *deleteButton = msgBox.addButton("Yes, Delete All", QMessageBox::YesRole);
    QPushButton *cancelButton = msgBox.addButton("No, Keep History", QMessageBox::NoRole);

    // Style the message box
    msgBox.setStyleSheet(
        "QMessageBox {"
        "background-color: #f8f9fa;"
        "border-radius: 10px;"
        "}"
        "QMessageBox QLabel {"
        "color: #2c3e50;"
        "font-size: 14px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "padding: 10px;"
        "}"
        "QPushButton {"
        "background-color: #007bff;"
        "color: white;"
        "border: none;"
        "border-radius: 6px;"
        "padding: 8px 20px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "min-width: 100px;"
        "margin: 5px;"
        "}"
        "QPushButton:hover {"
        "background-color: #0056b3;"
        "}"
        );

    // Style delete button as red (dangerous action)
    deleteButton->setStyleSheet(
        "QPushButton {"
        "background-color: #dc3545;"
        "color: white;"
        "border: none;"
        "border-radius: 6px;"
        "padding: 8px 20px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "min-width: 100px;"
        "margin: 5px;"
        "}"
        "QPushButton:hover {"
        "background-color: #c82333;"
        "}"
        );

    // Set default button to cancel (safer option)
    msgBox.setDefaultButton(cancelButton);

    // Execute dialog
    msgBox.exec();

    if (msgBox.clickedButton() == deleteButton) {
        try {
            if (database->deleteAllGamesForUser(currentUserId)) {
                // Custom success message
                QMessageBox successBox(this);
                successBox.setWindowTitle("✅ Success");
                successBox.setText("All game history deleted successfully!");
                successBox.setInformativeText("Your game history has been completely cleared.");
                successBox.setIcon(QMessageBox::Information);
                successBox.setStandardButtons(QMessageBox::Ok);
                successBox.setStyleSheet(
                    "QMessageBox {"
                    "background-color: #f8f9fa;"
                    "}"
                    "QMessageBox QLabel {"
                    "color: #28a745;"
                    "font-size: 14px;"
                    "font-weight: bold;"
                    "font-family: 'Segoe UI', Arial, sans-serif;"
                    "}"
                    "QPushButton {"
                    "background-color: #28a745;"
                    "color: white;"
                    "border: none;"
                    "border-radius: 6px;"
                    "padding: 8px 20px;"
                    "font-size: 14px;"
                    "font-weight: bold;"
                    "min-width: 80px;"
                    "}"
                    );
                successBox.exec();

                // Clear the current display
                clearBoard();
                currentMoves.clear();

                // Reload the history (which will now be empty)
                loadGameHistory();
                updateStats();

                // Reset replay controls
                onStopReplay();

            } else {
                // Error message
                QMessageBox errorBox(this);
                errorBox.setWindowTitle("❌ Error");
                errorBox.setText("Failed to delete game history!");
                errorBox.setInformativeText("An error occurred while trying to delete your game history. Please try again.");
                errorBox.setIcon(QMessageBox::Critical);
                errorBox.setStandardButtons(QMessageBox::Ok);
                errorBox.setStyleSheet(
                    "QMessageBox {"
                    "background-color: #f8f9fa;"
                    "}"
                    "QMessageBox QLabel {"
                    "color: #dc3545;"
                    "font-size: 14px;"
                    "font-weight: bold;"
                    "font-family: 'Segoe UI', Arial, sans-serif;"
                    "}"
                    "QPushButton {"
                    "background-color: #dc3545;"
                    "color: white;"
                    "border: none;"
                    "border-radius: 6px;"
                    "padding: 8px 20px;"
                    "font-size: 14px;"
                    "font-weight: bold;"
                    "min-width: 80px;"
                    "}"
                    );
                errorBox.exec();
            }
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Database Error", QString("Database error: %1").arg(e.what()));
        }
    }
    // If cancel button was clicked, do nothing (dialog just closes)
}


void HistoryWindow::loadGameHistory()
{
    try {
        gameHistory = database->getGameHistory(currentUserId);
        gameHistoryList->clear();

        if (gameHistory.empty()) {
            gameHistoryList->addItem("No games found");
            return;
        }

        QString filterText = gameModeFilter->currentText();

        // Handle all filter cases properly
        for (const auto& game : gameHistory) {
            QString gameMode = QString::fromStdString(game.gameMode);

            // Apply comprehensive filter logic
            bool shouldShow = false;

            if (filterText == "All Games") {
                shouldShow = true;
            } else if (filterText == "Classic" && gameMode == "Classic") {
                shouldShow = true;
            } else if (filterText == "Overwrite" && gameMode == "Overwrite") {
                shouldShow = true;
            } else if (filterText == "AI Easy" && gameMode == "AI Easy") {
                shouldShow = true;
            } else if (filterText == "AI Medium" && gameMode == "AI Medium") {
                shouldShow = true;
            } else if (filterText == "AI Hard" && gameMode == "AI Hard") {
                shouldShow = true;
            }

            // Skip this game if it doesn't match the filter
            if (!shouldShow) continue;

            QString currentUserName = QString::fromStdString(
                game.player1Id == currentUserId ?
                    database->getUsernameById(game.player1Id) :
                    database->getUsernameById(game.player2Id));

            QString opponent;

            // Format opponent name based on game mode
            if (gameMode.contains("AI")) {
                QString aiLevel;
                if (gameMode == "AI Easy") aiLevel = "Easy";
                else if (gameMode == "AI Medium") aiLevel = "Medium";
                else if (gameMode == "AI Hard") aiLevel = "Hard";
                else aiLevel = "AI";

                opponent = QString("%1 vs AI (%2)").arg(currentUserName).arg(aiLevel);
            } else {
                opponent = QString("%1 vs Guest (%2)").arg(currentUserName).arg(gameMode);
            }

            QString result;
            QColor itemColor;

            // Set colors based on game result
            if (game.winner == 0) {
                result = "Draw";
                itemColor = QColor("#fd7e14"); // Orange for draws
            } else if (game.winner == currentUserId) {
                result = "Win";
                itemColor = QColor("#28a745"); // Green for wins
            } else {
                result = "Loss";
                itemColor = QColor("#dc3545"); // Red for losses
            }

            QString itemText = QString("%1 - %2 (%3)")
                                   .arg(opponent)
                                   .arg(result)
                                   .arg(QString::fromStdString(game.timestamp).left(10));

            QListWidgetItem *item = new QListWidgetItem(itemText);
            item->setData(Qt::UserRole, game.id);

            // Apply the color to the item text
            item->setForeground(itemColor);

            gameHistoryList->addItem(item);
        }

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to load history: %1").arg(e.what()));
    }
}


void HistoryWindow::onGameSelected()
{
    QListWidgetItem *item = gameHistoryList->currentItem();
    if (!item) return;

    // Ensure the item stays selected
    item->setSelected(true);
    gameHistoryList->setCurrentItem(item);

    int gameId = item->data(Qt::UserRole).toInt();

    for (const auto& game : gameHistory) {
        if (game.id == gameId) {
            initializeReplay(game.moves);
            break;
        }
    }
}

void HistoryWindow::initializeReplay(const std::string& moves)
{
    currentMoves.clear();

    std::string movesStr = moves;
    size_t pos = 0;
    while ((pos = movesStr.find(',')) != std::string::npos) {
        currentMoves.push_back(movesStr.substr(0, pos));
        movesStr.erase(0, pos + 1);
    }
    if (!movesStr.empty()) {
        currentMoves.push_back(movesStr);
    }

    currentMoveIndex = 0;
    moveSlider->setMaximum(static_cast<int>(currentMoves.size()));
    moveSlider->setValue(0);

    resetReplay();
    updateReplayControls();
}

void HistoryWindow::displayMoveAtStep(int step)
{
    clearBoard();

    for (size_t i = 0; i < static_cast<size_t>(step) && i < currentMoves.size(); i++) {
        const std::string& move = currentMoves[i];
        if (move.length() >= 3) {
            char player = move[0];
            int row = move[1] - '0';
            int col = move[2] - '0';
            int position = row * 3 + col;

            if (position >= 0 && position < 9) {
                boardButtons[position]->setText(QString(player));
                if (player == 'X') {
                    // Red for X
                    boardButtons[position]->setStyleSheet(
                        "QPushButton {"
                        "background-color: #dc3545;"  // Red background
                        "border: 3px solid #c82333;"
                        "border-radius: 10px;"
                        "font-size: 42px;"
                        "font-weight: bold;"
                        "font-family: 'Arial', sans-serif;"
                        "color: white;"
                        "text-shadow: 1px 1px 2px rgba(0,0,0,0.3);"
                        "}"
                        );
                } else {
                    // Green for O
                    boardButtons[position]->setStyleSheet(
                        "QPushButton {"
                        "background-color: #4A90E2;"  // blue background
                        "border: 3px solid #3a7bc8;"
                        "border-radius: 10px;"
                        "font-size: 42px;"
                        "font-weight: bold;"
                        "font-family: 'Arial', sans-serif;"
                        "color: white;"
                        "text-shadow: 1px 1px 2px rgba(0,0,0,0.3);"
                        "}"
                        );
                }
            }
        }
    }
}


void HistoryWindow::clearBoard()
{
    for (int i = 0; i < 9; i++) {
        boardButtons[i]->setText("");
        boardButtons[i]->setStyleSheet(
            "QPushButton {"
            "background-color: #ffffff;"
            "border: 2px solid #4A90E2;"
            "border-radius: 10px;"
            "font-size: 42px;"
            "font-weight: bold;"
            "font-family: 'Arial', sans-serif;"
            "color: #495057;"
            "}"
            );
    }
}

void HistoryWindow::onPlayReplay()
{
    if (currentMoves.empty()) return;

    isReplaying = true;
    isPaused = false;
    replayTimer->start(replaySpeed);

    playButton->setEnabled(false);
    pauseButton->setEnabled(true);
    stopButton->setEnabled(true);
}

void HistoryWindow::onPauseReplay()
{
    isPaused = true;
    replayTimer->stop();

    playButton->setEnabled(true);
    pauseButton->setEnabled(false);
}

void HistoryWindow::onStopReplay()
{
    isReplaying = false;
    isPaused = false;
    replayTimer->stop();
    currentMoveIndex = 0;

    displayMoveAtStep(0);
    updateReplayControls();

    playButton->setEnabled(true);
    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);
}

void HistoryWindow::onNextMove()
{
    if (currentMoveIndex < static_cast<int>(currentMoves.size())) {
        currentMoveIndex++;
        displayMoveAtStep(currentMoveIndex);
        updateReplayControls();
    }
}

void HistoryWindow::onPreviousMove()
{
    if (currentMoveIndex > 0) {
        currentMoveIndex--;
        displayMoveAtStep(currentMoveIndex);
        updateReplayControls();
    }
}

void HistoryWindow::onSliderChanged(int value)
{
    currentMoveIndex = value;
    displayMoveAtStep(currentMoveIndex);
    updateReplayControls();
}

void HistoryWindow::onReplayStep()
{
    if (currentMoveIndex < static_cast<int>(currentMoves.size())) {
        currentMoveIndex++;
        displayMoveAtStep(currentMoveIndex);
        updateReplayControls();
    } else {
        onStopReplay();
    }
}

void HistoryWindow::onSpeedChanged(int speed)
{
    replaySpeed = 2000 / speed;
    speedLabel->setText(QString("Speed: %1x").arg(speed));

    if (replayTimer->isActive()) {
        replayTimer->setInterval(replaySpeed);
    }
}

void HistoryWindow::updateReplayControls()
{
    moveSlider->setValue(currentMoveIndex);
    moveLabel->setText(QString("Move: %1/%2").arg(currentMoveIndex).arg(static_cast<int>(currentMoves.size())));

    prevButton->setEnabled(currentMoveIndex > 0);
    nextButton->setEnabled(currentMoveIndex < static_cast<int>(currentMoves.size()));
}

void HistoryWindow::resetReplay()
{
    currentMoveIndex = 0;
    displayMoveAtStep(0);
    updateReplayControls();
}

void HistoryWindow::onRefreshHistory()
{
    loadGameHistory();
    updateStats();
}

void HistoryWindow::onFilterChanged()
{
    loadGameHistory();
}

void HistoryWindow::updateStats()
{
    try {
        auto stats = database->getUserStats(currentUserId);
        QString statsText = QString("Total Games: %1 | Wins: %2 | Losses: %3 | Draws: %4 | Win Rate: %5%")
                                .arg(stats.totalGames)
                                .arg(stats.wins)
                                .arg(stats.losses)
                                .arg(stats.draws)
                                .arg(QString::number(stats.winRate, 'f', 1));
        statsLabel->setText(statsText);
    } catch (const std::exception& e) {
        statsLabel->setText("Unable to load statistics");
    }
}
