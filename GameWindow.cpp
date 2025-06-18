#include "GameWindow.h"
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QGraphicsDropShadowEffect>


GameWindow::GameWindow(const QString& gameMode, QWidget *parent)
    : QWidget(parent), board(9, ' '), currentPlayer('X'), gameEnded(false),
    gameMode(gameMode), currentUserId(-1), database(nullptr), aiTimer(nullptr),
    isAIGame(false), aiPlayer('O'), humanPlayer('X'), aiDifficulty(1),
    overwriteGame(nullptr)
{
    // Initialize overwrite game if needed
    if (gameMode == "Overwrite Mode") {
        overwriteGame = new OverwriteGame();
    }

    try {
        database = new TicTacToeDB();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Database Error", e.what());
    }

    // Determine if this is an AI game
    if (gameMode.contains("AI")) {
        isAIGame = true;
        if (gameMode == "AI Easy") aiDifficulty = 1;
        else if (gameMode == "AI Medium") aiDifficulty = 2;
        else if (gameMode == "AI Hard") aiDifficulty = 3;

        aiTimer = new QTimer(this);
        aiTimer->setSingleShot(true);
        connect(aiTimer, &QTimer::timeout, this, &GameWindow::onAIMove);
    }

    setupUI();
    initializeGame();
}

GameWindow::~GameWindow()
{
    delete database;
    delete overwriteGame;
}

void GameWindow::setCurrentUser(const QString& username, int userId)
{
    currentUsername = username;
    currentUserId = userId;
}

void GameWindow::setupUI()
{
    resize(1000, 800);
    setWindowTitle("Tic Tac Toe - " + gameMode);

    // Center window on screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(25, 20, 25, 25);

    // Title
    titleLabel = new QLabel(gameMode);
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

    // Status
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

    // Board container - INCREASED SIZE
    boardContainer = new QFrame();
    boardContainer->setFixedSize(500, 500); // Changed from 350x350 to 500x500
    boardContainer->setStyleSheet(
        "QFrame {"
        "background-color: #f8f9fa;"
        "border-radius: 15px;"
        "border: 3px solid #dee2e6;"
        "padding: 10px;"
        "}"
        );

    QWidget *boardWidget = new QWidget(boardContainer);
    boardWidget->setGeometry(15, 15, 470, 470); // Adjusted to fit new container size
    boardLayout = new QGridLayout(boardWidget);
    boardLayout->setSpacing(5);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    // Create board buttons - INCREASED SIZE
    for (int i = 0; i < 9; i++) {
        buttons[i] = new QPushButton();
        buttons[i]->setFixedSize(150, 150); // Changed from 102x102 to 150x150
        buttons[i]->setStyleSheet(
            "QPushButton {"
            "background-color: #ffffff;"
            "border: 2px solid #4A90E2;"
            "border-radius: 10px;"
            "font-size: 60px;" // Increased font size from 80px to 60px
            "font-weight: bold;"
            "font-family: 'Arial', sans-serif;"
            "color: #495057;"
            "}"
            "QPushButton:hover {"
            "background-color: #e3f2fd;"
            "border-color: #2196F3;"
            "}"
            "QPushButton:pressed {"
            "background-color: #bbdefb;"
            "}"
            );

        connect(buttons[i], &QPushButton::clicked, this, &GameWindow::onCellClicked);
        boardLayout->addWidget(buttons[i], i/3, i%3);
    }

    // Control buttons - MATCHED WIDTH TO STATUS LABEL
    controlWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(controlWidget);
    buttonLayout->setSpacing(15);

    resetBtn = new QPushButton("🔄 Reset Game");
    resetBtn->setFixedHeight(50);
    // Remove setMinimumWidth to let it match the status label width
    resetBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #6c757d;"
        "color: white;"
        "border: none;"
        "border-radius: 12px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "padding: 8px 16px;"
        "border: 3px solid #5a6268;"
        "}"
        "QPushButton:hover {"
        "background-color: #5a6268;"
        "}"
        "QPushButton:pressed {"
        "background-color: #545b62;"
        "}"
        );

    backBtn = new QPushButton("⬅️ Back to Menu");
    backBtn->setFixedHeight(50);
    // Remove setMinimumWidth to let it match the status label width
    backBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #6c757d;"
        "color: white;"
        "border: none;"
        "border-radius: 12px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "padding: 8px 16px;"
        "border: 3px solid #5a6268;"
        "}"
        "QPushButton:hover {"
        "background-color: #5a6268;"
        "}"
        "QPushButton:pressed {"
        "background-color: #545b62;"
        "}"
        );

    connect(resetBtn, &QPushButton::clicked, this, &GameWindow::resetGame);
    connect(backBtn, &QPushButton::clicked, this, &GameWindow::backToMenu);

    buttonLayout->addWidget(resetBtn);
    buttonLayout->addWidget(backBtn);

    // Add components to main layout
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



void GameWindow::initializeGame()
{
    if (gameMode == "Overwrite Mode" && overwriteGame) {
        overwriteGame->initializeGame();
        currentPlayer = overwriteGame->getCurrentPlayer();
        gameEnded = overwriteGame->isGameEnded();
    } else {
        board = std::vector<char>(9, ' ');
        moveHistory.clear();
        gameEnded = false;
        currentPlayer = 'X';
    }

    statusLabel->setText("Player X's Turn");

    // Reset UI
    for (int i = 0; i < 9; i++) {
        buttons[i]->setText("");
        buttons[i]->setEnabled(true);
        buttons[i]->setStyleSheet(
            "QPushButton {"
            "background-color: #ffffff;"
            "border: 2px solid #4A90E2;"
            "border-radius: 10px;"
            "font-size: 80px;"
            "font-weight: bold;"
            "font-family: 'Arial', sans-serif;"
            "color: #495057;"
            "}"
            );
    }

    // Setup for AI games
    if (gameMode.contains("AI")) {
        isAIGame = true;
        aiPlayer = 'O';
        humanPlayer = 'X';
        aiDifficulty = gameMode.contains("Easy") ? 1 :
                           gameMode.contains("Medium") ? 2 : 3;

        if (currentPlayer == aiPlayer) {
            QTimer::singleShot(500, this, &GameWindow::onAIMove);
        }
    } else {
        isAIGame = false;
    }

    updateStatus("Current Turn: " + QString(currentPlayer));
}

void GameWindow::onCellClicked()
{
    if (gameEnded) return;

    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    // Find button position
    int position = -1;
    for (int i = 0; i < 9; i++) {
        if (buttons[i] == button) {
            position = i;
            break;
        }
    }

    if (position == -1) return;

    // Check if move is valid
    if (!isValidMove(position, currentPlayer)) {
        if (gameMode == "Overwrite Mode") {
            updateStatus("Invalid move! Cannot overwrite this cell yet.");
        }
        return;
    }

    // Make the move
    if (makeMove(position, currentPlayer)) {
        updateButtonDisplay(position);

        if (!gameEnded && isAIGame && currentPlayer == aiPlayer) {
            // Disable all buttons during AI turn
            for (int i = 0; i < 9; i++) {
                buttons[i]->setEnabled(false);
            }
            updateStatus("AI is thinking...");
            aiTimer->start(100); // 1 second delay
        }
    }
}

bool GameWindow::isValidMove(int position, char player)
{
    if (gameMode == "Overwrite Mode" && overwriteGame) {
        return overwriteGame->isValidMove(position, player);
    } else {
        // Regular game: only empty cells are valid
        return board[position] == ' ';
    }
}

bool GameWindow::makeMove(int position, char player)
{
    if (gameMode == "Overwrite Mode" && overwriteGame) {
        if (overwriteGame->makeMove(position, player)) {
            currentPlayer = overwriteGame->getCurrentPlayer();
            gameEnded = overwriteGame->isGameEnded();

            if (overwriteGame->checkWin(player)) {
                endGame(QString(player) + " Wins!");
            } else if (overwriteGame->checkDraw()) {
                endGame("Draw!");
            } else {
                updateStatus("Current Turn: " + QString(currentPlayer));
            }
            return true;
        }
        return false;
    } else {
        // Regular game logic
        if (gameEnded || board[position] != ' ') {
            return false;
        }

        board[position] = player;
        moveHistory.push_back(std::string(1, player) + std::to_string(position / 3) + std::to_string(position % 3));

        if (checkWin(player)) {
            endGame(QString(player) + " Wins!");
        } else if (checkDraw()) {
            endGame("Draw!");
        } else {
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            updateStatus("Current Turn: " + QString(currentPlayer));
        }
        return true;
    }
}

void GameWindow::updateButtonDisplay(int position)
{
    char cellValue;
    if (gameMode == "Overwrite Mode" && overwriteGame) {
        cellValue = overwriteGame->getCellValue(position);
    } else {
        cellValue = board[position];
    }

    buttons[position]->setText(QString(cellValue));

    // Apply styling based on player using the new GUI style
    if (cellValue == 'X') {
        buttons[position]->setStyleSheet(
            "QPushButton {"
            "background-color: #dc3545;"
            "border: 3px solid #c82333;"
            "border-radius: 10px;"
            "font-size: 80px;"
            "font-weight: bold;"
            "font-family: 'Arial', sans-serif;"
            "color: white;"
            "text-shadow: 1px 1px 2px rgba(0,0,0,0.3);"
            "}"
            );
    } else {
        buttons[position]->setStyleSheet(
            "QPushButton {"
            "background-color: #4A90E2;"
            "border: 3px solid #3a7bc8;"
            "border-radius: 10px;"
            "font-size: 80px;"
            "font-weight: bold;"
            "font-family: 'Arial', sans-serif;"
            "color: white;"
            "text-shadow: 1px 1px 2px rgba(0,0,0,0.3);"
            "}"
            );
    }
}

bool GameWindow::checkWin(char player)
{
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i*3] == player && board[i*3+1] == player && board[i*3+2] == player) {
            return true;
        }
    }

    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[i] == player && board[i+3] == player && board[i+6] == player) {
            return true;
        }
    }

    // Check diagonals
    if (board[0] == player && board[4] == player && board[8] == player) {
        return true;
    }
    if (board[2] == player && board[4] == player && board[6] == player) {
        return true;
    }

    return false;
}

bool GameWindow::checkDraw()
{
    for (int i = 0; i < 9; i++) {
        if (board[i] == ' ') {
            return false;
        }
    }
    return true;
}

void GameWindow::endGame(const QString& result)
{
    gameEnded = true;

    // Create enhanced win messages
    QString enhancedMessage;
    QString statusMessage;

    if (result.contains("X Wins")) {
        enhancedMessage = "🎉 VICTORY! 🎉\n\n Player ❌ is the Champion!";
        statusMessage = "🏆 Player X Wins! 🏆";
    }
    else if (result.contains("O Wins")) {
        enhancedMessage = "🎉 VICTORY! 🎉\n\n Player ⭕ is the Champion!";
        statusMessage = "🏆 Player O Wins! 🏆";
    }
    else if (result.contains("Draw")) {
        enhancedMessage = "🤝 DRAW GAME! 🤝";
        statusMessage = "⚖️ It's a Draw! ⚖️";
    }
    else {
        enhancedMessage = "🎮 GAME OVER 🎮\n\n" + result + "\n\nThanks for playing!";
        statusMessage = result;
    }

    updateStatus(statusMessage);

    // Disable all buttons
    for (int i = 0; i < 9; i++) {
        buttons[i]->setEnabled(false);
    }

    // Save game to database
    saveGameToDatabase();

    // === Delay before showing the message box ===
    int delayMs = 70; // يمكنك تعديل قيمة التأخير هنا بالمللي ثانية
    QTimer::singleShot(delayMs, this, [this, enhancedMessage]() {
        // Show enhanced message box with OK button to close
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("🎮 Game Result");
        msgBox.setText(enhancedMessage);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);  // Add OK button to close

        // Style the message box
        msgBox.setStyleSheet(
            "QMessageBox {"
            "background-color: #f8f9fa;"
            "border-radius: 10px;"
            "}"
            "QMessageBox QLabel {"
            "color: #2c3e50;"
            "font-size: 16px;"
            "font-weight: bold;"
            "font-family: 'Segoe UI', Arial, sans-serif;"
            "padding: 10px;"
            "}"
            "QPushButton {"
            "background-color: #3498db;"
            "color: white;"
            "border: none;"
            "border-radius: 8px;"
            "font-size: 14px;"
            "font-weight: bold;"
            "padding: 10px 20px;"
            "margin: 5px;"
            "}"
            "QPushButton:hover {"
            "background-color: #2980b9;"
            "}"
            );

        msgBox.exec();
    });

    // Stay in the same game window after closing
}




void GameWindow::updateStatus(const QString& message)
{
    statusLabel->setText(message);
}

void GameWindow::saveGameToDatabase()
{
    if (database && currentUserId != -1) {
        std::vector<std::string> finalMoveHistory;
        if (gameMode == "Overwrite Mode" && overwriteGame) {
            finalMoveHistory = overwriteGame->getMoveHistory();
        } else {
            finalMoveHistory = moveHistory;
        }

        // Determine winner based on game result
        int winner = 0; // 0 = draw, 1 = player1 wins, 2 = player2 wins
        if (!gameEnded) {
            winner = 0; // Game not finished
        } else {
            QString statusText = statusLabel->text();
            if (statusText.contains("X Wins")) {
                winner = 1; // Player X wins
            } else if (statusText.contains("O Wins")) {
                winner = 2; // Player O wins
            } else {
                winner = 0; // Draw
            }
        }

        // For single player games, use currentUserId for both players
        // For AI games, player1 is human, player2 is AI (-1)
        int player1Id = currentUserId;
        int player2Id = isAIGame ? -1 : currentUserId; // Use -1 for AI

        database->saveGame(player1Id, player2Id, winner, finalMoveHistory, gameMode.toStdString());
    }
}

void GameWindow::resetGame()
{
    initializeGame();
}

void GameWindow::backToMenu()
{
    emit backToMenuRequested();
}

void GameWindow::onAIMove()
{
    if (gameEnded || !isAIGame || currentPlayer != aiPlayer) {
        // Re-enable buttons for human player
        for (int i = 0; i < 9; i++) {
            if (isValidMove(i, humanPlayer)) {
                buttons[i]->setEnabled(true);
            }
        }
        return;
    }


    int aiMove = getAIMove();
    if (aiMove != -1) {
        makeMove(aiMove, aiPlayer);
        updateButtonDisplay(aiMove);
    }

    // Re-enable buttons for human player if game continues
    if (!gameEnded) {
        for (int i = 0; i < 9; i++) {
            if (isValidMove(i, humanPlayer)) {
                buttons[i]->setEnabled(true);
            }
        }
    }
}

int GameWindow::getAIMove()
{
    switch (aiDifficulty) {
    case 1: return getEasyAIMove();
    case 2: return getMediumAIMove();
    case 3: return getHardAIMove();
    default: return getEasyAIMove();
    }
}

int GameWindow::getEasyAIMove()
{
    std::vector<int> availableMoves;

     availableMoves = getAvailableMoves(board);

    if (availableMoves.empty()) return -1;

    int randomIndex = QRandomGenerator::global()->bounded(availableMoves.size());
    return availableMoves[randomIndex];
}

int GameWindow::getMediumAIMove()
{
    // Medium AI: 70% optimal, 30% random
    if (QRandomGenerator::global()->bounded(100) < 70) {
        return getHardAIMove();
    } else {
        return getEasyAIMove();
    }
}

int GameWindow::getHardAIMove()
{

        // Alpha-beta minimax for standard game
        std::vector<char> tempBoard = board;
        return minimax(tempBoard, 0, true, -1000, 1000);

}

int GameWindow::minimax(std::vector<char>& board, int depth, bool isMaximizing, int alpha, int beta)
{
    int score = evaluate(board);

    // Terminal conditions with depth preference for faster wins/slower losses
    if (score == 10) return score - depth;  // Prefer quicker wins
    if (score == -10) return score + depth; // Delay losses
    if (getAvailableMoves(board).empty()) return 0;

    if (isMaximizing) {
        int best = -1000;
        int bestMove = -1;

        for (int i = 0; i < 9; i++) {
            if (board[i] == ' ') {
                board[i] = aiPlayer;
                int moveScore = minimax(board, depth + 1, false, alpha, beta);
                board[i] = ' ';

                if (moveScore > best) {
                    best = moveScore;
                    bestMove = i;
                }

                alpha = std::max(alpha, best);

                // Alpha-beta pruning
                if (beta <= alpha) {
                    break; // Beta cutoff - prune remaining branches
                }
            }
        }

        return (depth == 0) ? bestMove : best;
    } else {
        int best = 1000;

        for (int i = 0; i < 9; i++) {
            if (board[i] == ' ') {
                board[i] = humanPlayer;
                int moveScore = minimax(board, depth + 1, true, alpha, beta);
                board[i] = ' ';

                best = std::min(best, moveScore);
                beta = std::min(beta, best);

                // Alpha-beta pruning
                if (beta <= alpha) {
                    break; // Alpha cutoff - prune remaining branches
                }
            }
        }

        return best;
    }
}


int GameWindow::evaluate(const std::vector<char>& gameBoard)
{
    // Check rows
    for (int row = 0; row < 3; row++) {
        if (gameBoard[row*3] == gameBoard[row*3+1] && gameBoard[row*3+1] == gameBoard[row*3+2]) {
            if (gameBoard[row*3] == aiPlayer) return 10;
            else if (gameBoard[row*3] == humanPlayer) return -10;
        }
    }

    // Check columns
    for (int col = 0; col < 3; col++) {
        if (gameBoard[col] == gameBoard[col+3] && gameBoard[col+3] == gameBoard[col+6]) {
            if (gameBoard[col] == aiPlayer) return 10;
            else if (gameBoard[col] == humanPlayer) return -10;
        }
    }

    // Check diagonals
    if (gameBoard[0] == gameBoard[4] && gameBoard[4] == gameBoard[8]) {
        if (gameBoard[0] == aiPlayer) return 10;
        else if (gameBoard[0] == humanPlayer) return -10;
    }

    if (gameBoard[2] == gameBoard[4] && gameBoard[4] == gameBoard[6]) {
        if (gameBoard[2] == aiPlayer) return 10;
        else if (gameBoard[2] == humanPlayer) return -10;
    }

    return 0;
}

std::vector<int> GameWindow::getAvailableMoves(const std::vector<char>& board)
{
    std::vector<int> moves;
    for (int i = 0; i < 9; i++) {
        if (board[i] == ' ') {
            moves.push_back(i);
        }
    }
    return moves;
}
