#include "MainWindow.h"
#include <QApplication>
#include <QFont>
#include <QScreen>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), database(nullptr), currentUserId(-1), isLoggedIn(false)
{
    try {
        database = new TicTacToeDB();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Database Error", e.what());
    }

    setupUI();
    applyModernStyling();
    showLogin();
}

MainWindow::~MainWindow()
{
    delete database;
}

void MainWindow::setupUI()
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupLoginMenu();
    setupMainMenu();
    setupAIMenu();

    setWindowTitle("Tic Tac Toe - Professional Edition");
    setFixedSize(500, 700);

    // Center window on screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void MainWindow::setupLoginMenu()
{
    loginWidget = new QWidget();
    loginLayout = new QVBoxLayout(loginWidget);
    loginLayout->setSpacing(0);
    loginLayout->setContentsMargins(0, 0, 0, 0);

    // Login Header - Match game window title styling
    QFrame *loginHeaderFrame = new QFrame();
    loginHeaderFrame->setFixedHeight(220); // Same height as game window header
    loginHeaderFrame->setStyleSheet(
        "QFrame {"
        "background-color: #4a90e2;" // Same blue as game window
        "border-radius: 0px;"
        "}"
        );

    QVBoxLayout *loginHeaderLayout = new QVBoxLayout(loginHeaderFrame);
    loginHeaderLayout->setAlignment(Qt::AlignCenter);

    // Title with game window styling
    loginTitleLabel = new QLabel("TIC TAC TOE");
    loginTitleLabel->setAlignment(Qt::AlignCenter);
    loginTitleLabel->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 36px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "margin: 10px;"
        "}"
        );

    // Subtitle
    QLabel *loginSubtitle = new QLabel("Professional Edition");
    loginSubtitle->setAlignment(Qt::AlignCenter);
    loginSubtitle->setStyleSheet(
        "QLabel {"
        "color: rgba(255, 255, 255, 0.9);"
        "font-size: 16px;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "margin-bottom: 20px;"
        "}"
        );

    // Welcome message
    QLabel *welcomeLabel = new QLabel("Welcome Back!");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet(
        "QLabel {"
        "color: rgba(255, 255, 255, 0.9);"
        "font-size: 14px;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        );

    loginHeaderLayout->addWidget(loginTitleLabel);
    loginHeaderLayout->addWidget(loginSubtitle);
    loginHeaderLayout->addWidget(welcomeLabel);

    // Login Buttons Frame - Match game window button frame
    QFrame *loginButtonFrame = new QFrame();
    loginButtonFrame->setStyleSheet("QFrame { background-color: #f5f7fa; }");
    loginButtonFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *loginButtonLayout = new QVBoxLayout(loginButtonFrame);
    loginButtonLayout->setSpacing(20); // Match game window button spacing
    loginButtonLayout->setContentsMargins(40, 40, 40, 40); // Match game window margins

    // Create buttons with EXACT game window styling
    loginBtn = createStyledButton("🔑 Login", "#3498db");
    registerBtn = createStyledButton("📝 Register", "#2ecc71");
    guestBtn = createStyledButton("👤 Continue as Guest", "#95a5a6");

    // Add game window button animations
    addButtonAnimation(loginBtn);
    addButtonAnimation(registerBtn);
    addButtonAnimation(guestBtn);

    loginButtonLayout->addWidget(loginBtn);
    loginButtonLayout->addWidget(registerBtn);
    loginButtonLayout->addWidget(guestBtn);
    loginButtonLayout->addStretch();

    // Use same layout proportions as game window
    loginLayout->addWidget(loginHeaderFrame, 1); // Same proportion as game window
    loginLayout->addWidget(loginButtonFrame, 2); // Same proportion as game window

    // Connect login signals (keep your existing connections)
    connect(loginBtn, &QPushButton::clicked, this, [this]() {
        if (authenticateUser()) {
            showMainMenu();
        }
    });

    connect(registerBtn, &QPushButton::clicked, this, [this]() {
        bool ok;
        QString username = QInputDialog::getText(this, "Register", "Enter username:", QLineEdit::Normal, "", &ok);
        if (!ok || username.isEmpty()) return;

        QString password = QInputDialog::getText(this, "Register", "Enter password:", QLineEdit::Password, "", &ok);
        if (!ok || password.isEmpty()) return;

        try {
            if (database->createUser(username.toStdString(), password.toStdString())) {
                QMessageBox::information(this, "Success", "Account created successfully!");
                currentUsername = username;
                currentUserId = database->getUserId(username.toStdString());
                isLoggedIn = true;
                showMainMenu();
            } else {
                QMessageBox::warning(this, "Error", "Failed to create account. Username might already exist.");
            }
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
        }
    });

    connect(guestBtn, &QPushButton::clicked, this, [this]() {
        currentUsername = "Guest";
        currentUserId = -1;
        isLoggedIn = false;
        showMainMenu();
    });

    stackedWidget->addWidget(loginWidget);
}


void MainWindow::setupMainMenu()
{
    mainMenuWidget = new QWidget();
    mainLayout = new QVBoxLayout(mainMenuWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Header Section
    headerFrame = new QFrame();
    headerFrame->setFixedHeight(220);
    headerFrame->setStyleSheet(
        "QFrame {"
        "background-color: #4a90e2;"
        "border-radius: 0px;"
        "}"
        );

    QVBoxLayout *headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setAlignment(Qt::AlignCenter);

    titleLabel = new QLabel("TIC TAC TOE");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 36px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "margin: 10px;"
        "}"
        );

    userLabel = new QLabel();
    userLabel->setAlignment(Qt::AlignCenter);
    userLabel->setStyleSheet(
        "QLabel {"
        "color: rgba(255, 255, 255, 0.9);"
        "font-size: 14px;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        );

    subtitleLabel = new QLabel("Choose Your Game Mode");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet(
        "QLabel {"
        "color: rgba(255, 255, 255, 0.9);"
        "font-size: 16px;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "margin-bottom: 20px;"
        "}"
        );

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(userLabel);
    headerLayout->addWidget(subtitleLabel);

    // Button Section
    buttonFrame = new QFrame();
    buttonFrame->setStyleSheet("QFrame { background-color: #f5f7fa; }");

    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonFrame);
    buttonLayout->setSpacing(20);
    buttonLayout->setContentsMargins(40, 40, 40, 40);

    // Create styled buttons (Settings button removed)
    classicGameBtn = createStyledButton("Classic Mode", "#2ecc71");
    overwriteGameBtn = createStyledButton("Overwrite Mode", "#3498db");
    aiGameBtn = createStyledButton("Play vs AI", "#e67e22");
    historyBtn = createStyledButton("Game History", "#9b59b6");
    logoutBtn = createStyledButton("Logout", "#34495e");
    exitBtn = createStyledButton("Exit", "#e74c3c");

    addButtonAnimation(classicGameBtn);
    addButtonAnimation(overwriteGameBtn);
    addButtonAnimation(aiGameBtn);
    addButtonAnimation(historyBtn);
    addButtonAnimation(logoutBtn);
    addButtonAnimation(exitBtn);

    buttonLayout->addWidget(classicGameBtn);
    buttonLayout->addWidget(overwriteGameBtn);
    buttonLayout->addWidget(aiGameBtn);
    buttonLayout->addWidget(historyBtn);
    buttonLayout->addWidget(logoutBtn);
    buttonLayout->addWidget(exitBtn);
    buttonLayout->addStretch();

    mainLayout->addWidget(headerFrame);
    mainLayout->addWidget(buttonFrame);

    // Connect signals (Settings signal removed)
    connect(classicGameBtn, &QPushButton::clicked, this, &MainWindow::startClassicGame);
    connect(overwriteGameBtn, &QPushButton::clicked, this, &MainWindow::startOverwriteGame);
    connect(aiGameBtn, &QPushButton::clicked, this, &MainWindow::showAIMenu);
    connect(historyBtn, &QPushButton::clicked, this, &MainWindow::showGameHistory);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::showLogin);
    connect(exitBtn, &QPushButton::clicked, qApp, &QApplication::quit);

    stackedWidget->addWidget(mainMenuWidget);
}

void MainWindow::setupAIMenu()
{
    aiMenuWidget = new QWidget();
    aiLayout = new QVBoxLayout(aiMenuWidget);
    aiLayout->setSpacing(0);
    aiLayout->setContentsMargins(0, 0, 0, 0);

    // AI Header
    QFrame *aiHeaderFrame = new QFrame();
    aiHeaderFrame->setFixedHeight(150);
    aiHeaderFrame->setStyleSheet(
        "QFrame {"
        "background-color: #3498db;"
        "}"
        );

    QVBoxLayout *aiHeaderLayout = new QVBoxLayout(aiHeaderFrame);
    aiHeaderLayout->setAlignment(Qt::AlignCenter);

    aiTitleLabel = new QLabel("AI DIFFICULTY");
    aiTitleLabel->setAlignment(Qt::AlignCenter);
    aiTitleLabel->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 28px;"
        "font-weight: bold;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        );

    aiHeaderLayout->addWidget(aiTitleLabel);

    // AI Buttons Section
    QFrame *aiButtonFrame = new QFrame();
    aiButtonFrame->setStyleSheet("QFrame { background-color: #f5f7fa; }");

    QVBoxLayout *aiButtonLayout = new QVBoxLayout(aiButtonFrame);
    aiButtonLayout->setSpacing(25);
    aiButtonLayout->setContentsMargins(40, 50, 40, 50);

    easyAIBtn = createStyledButton("Easy - Beginner", "#2ecc71");
    mediumAIBtn = createStyledButton("Medium - Intermediate", "#f39c12");
    hardAIBtn = createStyledButton("Hard - Expert", "#e74c3c");
    backToMainBtn = createStyledButton("Back to Main Menu", "#7f8c8d");

    addButtonAnimation(easyAIBtn);
    addButtonAnimation(mediumAIBtn);
    addButtonAnimation(hardAIBtn);
    addButtonAnimation(backToMainBtn);

    aiButtonLayout->addWidget(easyAIBtn);
    aiButtonLayout->addWidget(mediumAIBtn);
    aiButtonLayout->addWidget(hardAIBtn);
    aiButtonLayout->addStretch();
    aiButtonLayout->addWidget(backToMainBtn);

    aiLayout->addWidget(aiHeaderFrame);
    aiLayout->addWidget(aiButtonFrame);

    // Connect AI signals
    connect(easyAIBtn, &QPushButton::clicked, this, &MainWindow::startEasyAI);
    connect(mediumAIBtn, &QPushButton::clicked, this, &MainWindow::startMediumAI);
    connect(hardAIBtn, &QPushButton::clicked, this, &MainWindow::startHardAI);
    connect(backToMainBtn, &QPushButton::clicked, this, &MainWindow::showMainMenu);

    stackedWidget->addWidget(aiMenuWidget);
}

QPushButton* MainWindow::createStyledButton(const QString &text, const QString &color)
{
    QPushButton *button = new QPushButton(text);
    button->setFixedHeight(60);
    button->setCursor(Qt::PointingHandCursor);

    QString buttonStyle = QString(
                              "QPushButton {"
                              "background-color: %1;"
                              "border: none;"
                              "color: white;"
                              "padding: 15px;"
                              "text-align: center;"
                              "font-size: 16px;"
                              "font-weight: bold;"
                              "font-family: 'Segoe UI', Arial, sans-serif;"
                              "border-radius: 12px;"
                              "}"
                              "QPushButton:hover {"
                              "background-color: %2;"
                              "}"
                              "QPushButton:pressed {"
                              "background-color: %3;"
                              "}"
                              ).arg(color)
                              .arg(color == "#2ecc71" ? "#27ae60" :
                                       color == "#3498db" ? "#2980b9" :
                                       color == "#e67e22" ? "#d35400" :
                                       color == "#9b59b6" ? "#8e44ad" :
                                       color == "#7f8c8d" ? "#6c7b7d" :
                                       color == "#e74c3c" ? "#c0392b" :
                                       color == "#f39c12" ? "#e67e22" :
                                       color == "#34495e" ? "#2c3e50" :
                                       color == "#95a5a6" ? "#7f8c8d" : color)
                              .arg(color == "#2ecc71" ? "#229954" :
                                       color == "#3498db" ? "#21618c" :
                                       color == "#e67e22" ? "#ba4a00" :
                                       color == "#9b59b6" ? "#7d3c98" :
                                       color == "#7f8c8d" ? "#5d6d7e" :
                                       color == "#e74c3c" ? "#a93226" :
                                       color == "#f39c12" ? "#d68910" :
                                       color == "#34495e" ? "#1b2631" :
                                       color == "#95a5a6" ? "#6c7b7d" : color);

    button->setStyleSheet(buttonStyle);

    // Add shadow effect
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 5);
    button->setGraphicsEffect(shadow);

    return button;
}

void MainWindow::addButtonAnimation(QPushButton *button)
{
    button->setProperty("originalHeight", button->height());
}

void MainWindow::applyModernStyling()
{
    setStyleSheet(
        "QMainWindow {"
        "background-color: #ffffff;"
        "}"
        );
}

bool MainWindow::authenticateUser()
{
    bool ok;
    QString username = QInputDialog::getText(this, "Login", "Enter username:", QLineEdit::Normal, "", &ok);
    if (!ok || username.isEmpty()) return false;

    QString password = QInputDialog::getText(this, "Login", "Enter password:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return false;

    try {
        if (database->validateUser(username.toStdString(), password.toStdString())) {
            currentUsername = username;
            currentUserId = database->getUserId(username.toStdString());
            isLoggedIn = true;
            return true;
        } else {
            QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
            return false;
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
        return false;
    }
}

// Slot implementations
void MainWindow::showLogin()
{
    currentUsername = "";
    currentUserId = -1;
    isLoggedIn = false;
    stackedWidget->setCurrentWidget(loginWidget);
}

void MainWindow::showMainMenu()
{
    userLabel->setText("Welcome, " + currentUsername + "!");
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

void MainWindow::startClassicGame()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("Classic Mode", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::startOverwriteGame()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("Overwrite Mode", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::startEasyAI()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("AI Easy", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::startMediumAI()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("AI Medium", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::startHardAI()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("AI Hard", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::showAIMenu()
{
    stackedWidget->setCurrentWidget(aiMenuWidget);
}

void MainWindow::showGameHistory()
{
    if (!isLoggedIn) {
        QMessageBox::information(this, "Login Required", "Please login to view game history.");
        return;
    }

    HistoryWindow *historyWindow = new HistoryWindow(currentUsername, this);
    historyWindow->setAttribute(Qt::WA_DeleteOnClose);
    historyWindow->exec();
}
