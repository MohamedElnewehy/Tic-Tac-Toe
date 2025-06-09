#include "GameWindow.h"
#include "GameHistoryManager.h"
#include "overwrite_game.h"

GameWindow::GameWindow(const QString &gameMode, QWidget *parent)
    : QWidget(parent), currentGameMode(gameMode), aiDifficulty(1), isAIGame(false),
    currentUserId(-1), player2Id(-1), gameEnded(false)
{
    if (gameMode.contains("AI")) {
        isAIGame = true;
        if (gameMode.contains("Easy")) aiDifficulty = 1;
        else if (gameMode.contains("Medium")) aiDifficulty = 2;
        else if (gameMode.contains("Hard")) aiDifficulty = 3;
    }

    initializeBoard(&game);
    setupUI();
}

GameWindow::~GameWindow()
{
}

void GameWindow::setCurrentUser(const QString& username, int userId)
{
    currentUsername = username;
    currentUserId = userId;

    // Start recording game for history
    if (userId != -1) {
        GameHistoryManager* historyManager = GameHistoryManager::getInstance();
        historyManager->startNewGame(currentGameMode.toStdString(), userId,
                                     isAIGame ? -1 : player2Id);
    }
}

void GameWindow::setPlayer2(const QString& username, int userId)
{
    player2Username = username;
    player2Id = userId;
}

void GameWindow::setupUI()
{
    setWindowTitle("Tic Tac Toe - " + currentGameMode);
    setFixedSize(450, 650);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(25, 20, 25, 25);

    // Title Section
    titleLabel = new QLabel(currentGameMode);
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

    // Status Section
    statusLabel = new QLabel("Player X's Turn");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setFixedHeight(32);
    statusLabel->setStyleSheet(
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

    // Game Board Container
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

    // Grid Widget
    gridWidget = new QWidget(boardContainer);
    gridWidget->setGeometry(15, 15, 320, 320);
    gridLayout = new QGridLayout(gridWidget);
    gridLayout->setSpacing(5);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    // Create 3x3 grid
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            gridButtons[i][j] = new QPushButton();
            gridButtons[i][j]->setFixedSize(102, 102);
            gridButtons[i][j]->setProperty("row", i);
            gridButtons[i][j]->setProperty("col", j);

            gridButtons[i][j]->setStyleSheet(
                "QPushButton {"
                "background-color: #ffffff;"
                "border: 2px solid #adb5bd;"
                "border-radius: 10px;"
                "font-size: 42px;"
                "font-weight: bold;"
                "font-family: 'Arial', sans-serif;"
                "color: #495057;"
                "}"
                "QPushButton:hover {"
                "background-color: #f1f3f4;"
                "border-color: #007bff;"
                "transform: scale(1.02);"
                "}"
                "QPushButton:pressed {"
                "background-color: #e2e6ea;"
                "transform: scale(0.98);"
                "}"
                );

            connect(gridButtons[i][j], &QPushButton::clicked, this, &GameWindow::cellClicked);
            gridLayout->addWidget(gridButtons[i][j], i, j);
        }
    }

    // Control Buttons
    QWidget *controlWidget = new QWidget();
    controlWidget->setFixedHeight(70);
    controlWidget->setStyleSheet("QWidget { background-color: transparent; }");

    buttonLayout = new QHBoxLayout(controlWidget);
    buttonLayout->setSpacing(20);
    buttonLayout->setContentsMargins(10, 10, 10, 10);

    resetBtn = new QPushButton("🔄 New Game");
    resetBtn->setFixedSize(180, 60);
    resetBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #dc3545;"
        "color: white;"
        "border: none;"
        "border-radius: 15px;"
        "font-size: 17px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "padding: 18px;"
        "border: 3px solid #c82333;"
        "}"
        "QPushButton:hover {"
        "background-color: #c82333;"
        "transform: translateY(-3px);"
        "box-shadow: 0 4px 8px rgba(0,0,0,0.2);"
        "}"
        "QPushButton:pressed {"
        "background-color: #bd2130;"
        "transform: translateY(0px);"
        "}"
        );

    backBtn = new QPushButton("⬅ Main Menu");
    backBtn->setFixedSize(180, 60);
    backBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #6c757d;"
        "color: white;"
        "border: none;"
        "border-radius: 15px;"
        "font-size: 17px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "padding: 18px;"
        "border: 3px solid #5a6268;"
        "}"
        "QPushButton:hover {"
        "background-color: #5a6268;"
        "transform: translateY(-3px);"
        "box-shadow: 0 4px 8px rgba(0,0,0,0.2);"
        "}"
        "QPushButton:pressed {"
        "background-color: #545b62;"
        "transform: translateY(0px);"
        "}"
        );

    connect(resetBtn, &QPushButton::clicked, this, &GameWindow::resetGame);
    connect(backBtn, &QPushButton::clicked, this, &GameWindow::backToMenu);

    buttonLayout->addWidget(resetBtn);
    buttonLayout->addWidget(backBtn);

    // Add all components to main layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(statusLabel);
    mainLayout->addSpacing(8);
    mainLayout->addWidget(boardContainer, 0, Qt::AlignCenter);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(controlWidget);
    mainLayout->addSpacing(5);

    // Background styling
    setStyleSheet(
        "QWidget {"
        "background-color: #f8f9fa;"
        "}"
        );
}

void GameWindow::cellClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button || !game.gameActive || gameEnded) return;

    int row = button->property("row").toInt();
    int col = button->property("col").toInt();

    bool moveSuccess = false;

    // Handle different game modes
    if (currentGameMode.contains("Overwrite")) {
        // For overwrite mode, check if move is valid
        if (game.board[row][col] != ' ') {
            // Check if we can overwrite (not opponent's last move)
            LastMoves lastMoves; // You'll need to maintain this state
            if (canOverwrite(&game, &lastMoves, row, col)) {
                moveSuccess = makeMove(&game, row, col, true);
                if (moveSuccess) {
                    if (game.currentPlayer == 'X') {
                        lastMoves.lastPlayerXRow = row;
                        lastMoves.lastPlayerXCol = col;
                    } else {
                        lastMoves.lastPlayerORow = row;
                        lastMoves.lastPlayerOCol = col;
                    }
                }
            } else {
                QMessageBox::warning(this, "Invalid Move", "Cannot overwrite opponent's last move!");
                return;
            }
        } else {
            moveSuccess = makeMove(&game, row, col, true);
        }
    } else {
        // Classic mode and AI mode
        if (game.board[row][col] != ' ') {
            return; // Cell already occupied
        }
        moveSuccess = makeMove(&game, row, col, false);
    }

    if (!moveSuccess) return;

    // Record move for history
    if (currentUserId != -1) {
        GameHistoryManager* historyManager = GameHistoryManager::getInstance();
        historyManager->recordMove(game.currentPlayer, row, col);
    }

    updateCell(row, col);
    checkGameEnd();

    // Handle AI move with delay for better UX
    if (game.gameActive && !gameEnded && isAIGame && game.currentPlayer == 'O') {
        statusLabel->setText("🤖 AI is thinking...");
        statusLabel->setStyleSheet(
            "QLabel {"
            "background-color: #fd7e14;"
            "color: white;"
            "font-size: 15px;"
            "font-weight: bold;"
            "font-family: 'Segoe UI', Arial, sans-serif;"
            "border-radius: 8px;"
            "padding: 6px;"
            "border: 2px solid #e8690b;"
            "}"
            );

        QTimer::singleShot(1200, this, &GameWindow::makeAIMove);
    }
}

void GameWindow::updateCell(int row, int col)
{
    QString cellText = QString(game.board[row][col]);
    gridButtons[row][col]->setText(cellText);

    if (game.board[row][col] == 'X') {
        gridButtons[row][col]->setStyleSheet(
            "QPushButton {"
            "background-color: #dc3545;"
            "border: 3px solid #c82333;"
            "border-radius: 10px;"
            "font-size: 42px;"
            "font-weight: bold;"
            "font-family: 'Arial', sans-serif;"
            "color: white;"
            "text-shadow: 1px 1px 2px rgba(0,0,0,0.3);"
            "}"
            );
    } else if (game.board[row][col] == 'O') {
        gridButtons[row][col]->setStyleSheet(
            "QPushButton {"
            "background-color: #007bff;"
            "border: 3px solid #0056b3;"
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

void GameWindow::checkGameEnd()
{
    if (gameEnded) return;

    GameHistoryManager* historyManager = GameHistoryManager::getInstance();

    if (checkWin(&game)) {
        gameEnded = true;
        QString winner;
        QString winnerColor;
        int winnerId = 0;

        if (game.currentPlayer == 'X') {
            winner = "🎉 Player X Wins!";
            winnerColor = "#28a745";
            winnerId = currentUserId;
        } else if (isAIGame && game.currentPlayer == 'O') {
            winner = "🤖 AI Wins!";
            winnerColor = "#dc3545";
            winnerId = -1;
        } else {
            winner = "🎉 Player O Wins!";
            winnerColor = "#28a745";
            winnerId = player2Id != -1 ? player2Id : currentUserId;
        }

        // Save game result to history
        if (currentUserId != -1) {
            historyManager->saveGameResult(winnerId);
        }

        statusLabel->setText(winner);
        statusLabel->setStyleSheet(
            QString("QLabel {"
                    "background-color: %1;"
                    "color: white;"
                    "font-size: 15px;"
                    "font-weight: bold;"
                    "font-family: 'Segoe UI', Arial, sans-serif;"
                    "border-radius: 8px;"
                    "padding: 6px;"
                    "border: 2px solid #1e7e34;"
                    "}").arg(winnerColor)
            );

        // Create custom styled message box
        QMessageBox *gameOverBox = new QMessageBox(this);
        gameOverBox->setWindowTitle("🎮 Game Over");
        gameOverBox->setText(winner);
        gameOverBox->setStandardButtons(QMessageBox::Ok);
        gameOverBox->setStyleSheet(
            "QMessageBox {"
            "background-color: #f8f9fa;"
            "border-radius: 10px;"
            "}"
            "QMessageBox QLabel {"
            "color: #2c3e50;"
            "font-size: 16px;"
            "font-weight: bold;"
            "font-family: 'Segoe UI', Arial, sans-serif;"
            "padding: 20px;"
            "}"
            "QPushButton {"
            "background-color: #007bff;"
            "color: white;"
            "border: none;"
            "border-radius: 6px;"
            "padding: 8px 20px;"
            "font-size: 14px;"
            "font-weight: bold;"
            "min-width: 80px;"
            "}"
            "QPushButton:hover {"
            "background-color: #0056b3;"
            "}"
            );

        gameOverBox->exec();
        delete gameOverBox;

        game.gameActive = false;

        // Disable all buttons
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                gridButtons[i][j]->setEnabled(false);
                QString currentStyle = gridButtons[i][j]->styleSheet();
                gridButtons[i][j]->setStyleSheet(currentStyle +
                                                 "QPushButton { opacity: 0.6; }");
            }
        }
        return;
    }

    if (checkDraw(&game)) {
        gameEnded = true;

        // Save draw result to history
        if (currentUserId != -1) {
            historyManager->saveGameResult(0);
        }

        statusLabel->setText("🤝 It's a Draw!");
        statusLabel->setStyleSheet(
            "QLabel {"
            "background-color: #fd7e14;"
            "color: white;"
            "font-size: 15px;"
            "font-weight: bold;"
            "font-family: 'Segoe UI', Arial, sans-serif;"
            "border-radius: 8px;"
            "padding: 6px;"
            "border: 2px solid #e8690b;"
            "}"
            );

        // Create custom styled draw message box
        QMessageBox *drawBox = new QMessageBox(this);
        drawBox->setWindowTitle("🎮 Game Over");
        drawBox->setText("🤝 It's a Draw!");
        drawBox->setStandardButtons(QMessageBox::Ok);
        drawBox->setStyleSheet(
            "QMessageBox {"
            "background-color: #f8f9fa;"
            "border-radius: 10px;"
            "}"
            "QMessageBox QLabel {"
            "color: #2c3e50;"
            "font-size: 16px;"
            "font-weight: bold;"
            "font-family: 'Segoe UI', Arial, sans-serif;"
            "padding: 20px;"
            "}"
            "QPushButton {"
            "background-color: #fd7e14;"
            "color: white;"
            "border: none;"
            "border-radius: 6px;"
            "padding: 8px 20px;"
            "font-size: 14px;"
            "font-weight: bold;"
            "min-width: 80px;"
            "}"
            "QPushButton:hover {"
            "background-color: #e8690b;"
            "}"
            );

        drawBox->exec();
        delete drawBox;

        game.gameActive = false;
        return;
    }

    switchPlayer(&game);
    updateStatusLabel();
}

void GameWindow::updateStatusLabel()
{
    QString nextPlayer;
    QString statusColor;

    if (game.currentPlayer == 'X') {
        nextPlayer = "🎯 Player X's Turn";
        statusColor = "#dc3545";
    } else if (isAIGame && game.currentPlayer == 'O') {
        nextPlayer = "🤖 AI's Turn";
        statusColor = "#fd7e14";
    } else {
        nextPlayer = "🎯 Player O's Turn";
        statusColor = "#007bff";
    }

    statusLabel->setText(nextPlayer);
    statusLabel->setStyleSheet(
        QString("QLabel {"
                "background-color: %1;"
                "color: white;"
                "font-size: 15px;"
                "font-weight: bold;"
                "font-family: 'Segoe UI', Arial, sans-serif;"
                "border-radius: 8px;"
                "padding: 6px;"
                "border: 2px solid rgba(0,0,0,0.1);"
                "}").arg(statusColor)
        );
}

void GameWindow::makeAIMove()
{
    if (!game.gameActive || gameEnded) return;

    AIMove aiMove = getAIMove(&game, static_cast<AILevel>(aiDifficulty));

    if (aiMove.row != -1 && aiMove.col != -1) {
        makeMove(&game, aiMove.row, aiMove.col, false);

        // Record AI move for history
        if (currentUserId != -1) {
            GameHistoryManager* historyManager = GameHistoryManager::getInstance();
            historyManager->recordMove('O', aiMove.row, aiMove.col);
        }

        updateCell(aiMove.row, aiMove.col);
        checkGameEnd();
    }
}

void GameWindow::resetGame()
{
    // Clear current game and start new one for history
    if (currentUserId != -1) {
        GameHistoryManager* historyManager = GameHistoryManager::getInstance();
        historyManager->clearCurrentGame();
        historyManager->startNewGame(currentGameMode.toStdString(), currentUserId,
                                     isAIGame ? -1 : player2Id);
    }

    initializeBoard(&game);
    gameEnded = false;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            gridButtons[i][j]->setText("");
            gridButtons[i][j]->setEnabled(true);
            gridButtons[i][j]->setStyleSheet(
                "QPushButton {"
                "background-color: #ffffff;"
                "border: 2px solid #adb5bd;"
                "border-radius: 10px;"
                "font-size: 42px;"
                "font-weight: bold;"
                "font-family: 'Arial', sans-serif;"
                "color: #495057;"
                "}"
                "QPushButton:hover {"
                "background-color: #f1f3f4;"
                "border-color: #007bff;"
                "transform: scale(1.02);"
                "}"
                "QPushButton:pressed {"
                "background-color: #e2e6ea;"
                "transform: scale(0.98);"
                "}"
                );
        }
    }

    updateStatusLabel();
}

void GameWindow::backToMenu()
{
    this->close();
}
