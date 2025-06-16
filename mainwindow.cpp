#include "MainWindow.h"
#include <QApplication>
#include <QFont>
#include <QScreen>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), database(nullptr), currentUserId(-1), isLoggedIn(false), wrongPasswordAttempts(0)
{
    try {
        database = new TicTacToeDB();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Database Error", e.what());
    }

    setupUI();
    applyModernStyling();
    checkForSavedSession();
}

MainWindow::~MainWindow()
{
    delete database;
}

void MainWindow::checkForSavedSession()
{
    if (database && database->hasActiveSession()) {
        QString savedUsername = QString::fromStdString(database->getLastLoggedUser());
        if (!savedUsername.isEmpty()) {
            currentUsername = savedUsername;
            currentUserId = database->getUserId(savedUsername.toStdString());
            isLoggedIn = true;
            showMainMenu();
            return;
        }
    }
    showLogin();
}

void MainWindow::setupUI()
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    setupLoginMenu();
    setupMainMenu();
    setupAIMenu();

    setWindowTitle("Tic Tac Toe - Professional Edition");
    resize(1000, 800);

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

    QFrame *loginHeaderFrame = new QFrame();
    loginHeaderFrame->setFixedHeight(220);
    loginHeaderFrame->setStyleSheet(
        "QFrame {"
        "background-color: #4a90e2;"
        "border-radius: 0px;"
        "}"
        );

    QVBoxLayout *loginHeaderLayout = new QVBoxLayout(loginHeaderFrame);
    loginHeaderLayout->setAlignment(Qt::AlignCenter);

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

    QFrame *loginButtonFrame = new QFrame();
    loginButtonFrame->setStyleSheet("QFrame { background-color: #f5f7fa; }");
    loginButtonFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *loginButtonLayout = new QVBoxLayout(loginButtonFrame);
    loginButtonLayout->setSpacing(20);
    loginButtonLayout->setContentsMargins(40, 40, 40, 40);

    loginBtn = createStyledButton("🔑 Login", "#3498db");
    registerBtn = createStyledButton("📝 Register", "#2ecc71");
    guestBtn = createStyledButton("👤 Continue as Guest", "#95a5a6");
    forgotPasswordBtn = createStyledButton("🔒 Forgot Password?", "#e67e22");
    forgotPasswordBtn->hide();

    addButtonAnimation(loginBtn);
    addButtonAnimation(registerBtn);
    addButtonAnimation(guestBtn);
    addButtonAnimation(forgotPasswordBtn);

    loginButtonLayout->addWidget(loginBtn);
    loginButtonLayout->addWidget(registerBtn);
    loginButtonLayout->addWidget(guestBtn);
    loginButtonLayout->addWidget(forgotPasswordBtn);
    loginButtonLayout->addStretch();

    loginLayout->addWidget(loginHeaderFrame, 1);
    loginLayout->addWidget(loginButtonFrame, 2);

    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::authenticateUser);
    connect(registerBtn, &QPushButton::clicked, this, &MainWindow::registerUser);
    connect(guestBtn, &QPushButton::clicked, this, [this]() {
        currentUsername = "Guest";
        currentUserId = -1;
        isLoggedIn = false;
        showMainMenu();
    });
    connect(forgotPasswordBtn, &QPushButton::clicked, this, &MainWindow::forgotPassword);

    stackedWidget->addWidget(loginWidget);
}

void MainWindow::setupMainMenu()
{
    mainMenuWidget = new QWidget();
    mainLayout = new QVBoxLayout(mainMenuWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

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

    buttonFrame = new QFrame();
    buttonFrame->setStyleSheet("QFrame { background-color: #f5f7fa; }");

    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonFrame);
    buttonLayout->setSpacing(20);
    buttonLayout->setContentsMargins(40, 40, 40, 40);

    classicGameBtn = createStyledButton("Classic Mode", "#2ecc71");
    overwriteGameBtn = createStyledButton("Overwrite Mode", "#3498db");
    aiGameBtn = createStyledButton("Play vs AI", "#e67e22");
    historyBtn = createStyledButton("Game History", "#9b59b6");
    signOutBtn = createStyledButton("Sign Out", "#34495e");
    exitBtn = createStyledButton("Exit", "#e74c3c");

    addButtonAnimation(classicGameBtn);
    addButtonAnimation(overwriteGameBtn);
    addButtonAnimation(aiGameBtn);
    addButtonAnimation(historyBtn);
    addButtonAnimation(signOutBtn);
    addButtonAnimation(exitBtn);

    buttonLayout->addWidget(classicGameBtn);
    buttonLayout->addWidget(overwriteGameBtn);
    buttonLayout->addWidget(aiGameBtn);
    buttonLayout->addWidget(historyBtn);
    buttonLayout->addWidget(signOutBtn);
    buttonLayout->addWidget(exitBtn);
    buttonLayout->addStretch();

    mainLayout->addWidget(headerFrame);
    mainLayout->addWidget(buttonFrame);

    connect(classicGameBtn, &QPushButton::clicked, this, &MainWindow::startClassicGame);
    connect(overwriteGameBtn, &QPushButton::clicked, this, &MainWindow::startOverwriteGame);
    connect(aiGameBtn, &QPushButton::clicked, this, &MainWindow::showAIMenu);
    connect(historyBtn, &QPushButton::clicked, this, &MainWindow::showGameHistory);
    connect(signOutBtn, &QPushButton::clicked, this, &MainWindow::signOut);
    connect(exitBtn, &QPushButton::clicked, qApp, &QApplication::quit);

    stackedWidget->addWidget(mainMenuWidget);
}

void MainWindow::setupAIMenu()
{
    aiMenuWidget = new QWidget();
    aiLayout = new QVBoxLayout(aiMenuWidget);
    aiLayout->setSpacing(0);
    aiLayout->setContentsMargins(0, 0, 0, 0);

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
            wrongPasswordAttempts = 0;
            forgotPasswordBtn->hide();

            database->saveUserSession(username.toStdString());
            showMainMenu();
            return true;
        } else {
            wrongPasswordAttempts++;
            if (wrongPasswordAttempts >= 1) {
                forgotPasswordBtn->show();
            }
            QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
            return false;
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
        return false;
    }
}

bool MainWindow::registerUser()
{
    bool ok;
    QString username = QInputDialog::getText(this, "Register", "Enter username:", QLineEdit::Normal, "", &ok);
    if (!ok || username.isEmpty()) return false;

    if (database->userExists(username.toStdString())) {
        QMessageBox::warning(this, "Registration Failed", "Username is already taken. Please choose a different username.");
        return false;
    }

    QString password = QInputDialog::getText(this, "Register", "Enter password (minimum 4 characters):", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return false;

    if (password.length() < 4) {
        QMessageBox::warning(this, "Registration Failed", "Password must be at least 4 characters long.");
        return false;
    }

    QString favoriteColor = QInputDialog::getText(this, "Security Question 1", "What is your favorite color?", QLineEdit::Normal, "", &ok);
    if (!ok || favoriteColor.isEmpty()) return false;

    QString favoriteMeal = QInputDialog::getText(this, "Security Question 2", "What is your favorite meal?", QLineEdit::Normal, "", &ok);
    if (!ok || favoriteMeal.isEmpty()) return false;

    QString favoriteWord = QInputDialog::getText(this, "Security Question 3", "What is your favorite word?", QLineEdit::Normal, "", &ok);
    if (!ok || favoriteWord.isEmpty()) return false;

    QString birthPlace = QInputDialog::getText(this, "Security Question 4", "Where were you born?", QLineEdit::Normal, "", &ok);
    if (!ok || birthPlace.isEmpty()) return false;

    try {
        if (database->createUser(username.toStdString(), password.toStdString(),
                                 favoriteColor.toStdString(), favoriteMeal.toStdString(),
                                 favoriteWord.toStdString(), birthPlace.toStdString())) {
            QMessageBox::information(this, "Success", "Account created successfully! You are now logged in.");
            currentUsername = username;
            currentUserId = database->getUserId(username.toStdString());
            isLoggedIn = true;

            database->saveUserSession(username.toStdString());
            showMainMenu();
            return true;
        } else {
            QMessageBox::warning(this, "Error", "Failed to create account.");
            return false;
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
        return false;
    }
}

void MainWindow::forgotPassword()
{
    bool ok;
    QString username = QInputDialog::getText(this, "Forgot Password", "Enter your username:", QLineEdit::Normal, "", &ok);
    if (!ok || username.isEmpty()) return;

    if (!database->userExists(username.toStdString())) {
        QMessageBox::warning(this, "User Not Found", "Username does not exist.");
        return;
    }

    if (validateSecurityQuestions(username)) {
        QString newPassword = QInputDialog::getText(this, "Reset Password", "Enter new password (minimum 4 characters):", QLineEdit::Password, "", &ok);
        if (!ok || newPassword.isEmpty()) return;

        if (newPassword.length() < 4) {
            QMessageBox::warning(this, "Invalid Password", "Password must be at least 4 characters long.");
            return;
        }

        if (database->updatePassword(username.toStdString(), newPassword.toStdString())) {
            QMessageBox::information(this, "Success", "Password changed successfully! You are now logged in.");
            currentUsername = username;
            currentUserId = database->getUserId(username.toStdString());
            isLoggedIn = true;
            forgotPasswordBtn->hide();
            wrongPasswordAttempts = 0;

            database->saveUserSession(username.toStdString());
            showMainMenu();
        } else {
            QMessageBox::critical(this, "Error", "Failed to update password.");
        }
    }
}

bool MainWindow::validateSecurityQuestions(const QString& username)
{
    bool ok;
    QString favoriteColor = QInputDialog::getText(this, "Security Question 1", "What is your favorite color?", QLineEdit::Normal, "", &ok);
    if (!ok) return false;

    QString favoriteMeal = QInputDialog::getText(this, "Security Question 2", "What is your favorite meal?", QLineEdit::Normal, "", &ok);
    if (!ok) return false;

    QString favoriteWord = QInputDialog::getText(this, "Security Question 3", "What is your favorite word?", QLineEdit::Normal, "", &ok);
    if (!ok) return false;

    QString birthPlace = QInputDialog::getText(this, "Security Question 4", "Where were you born?", QLineEdit::Normal, "", &ok);
    if (!ok) return false;

    if (database->validateSecurityAnswers(username.toStdString(), favoriteColor.toStdString(),
                                          favoriteMeal.toStdString(), favoriteWord.toStdString(), birthPlace.toStdString())) {
        return true;
    } else {
        QMessageBox::warning(this, "Security Questions Failed", "Incorrect answers to security questions.");
        return false;
    }
}

void MainWindow::signOut()
{
    database->clearUserSession();
    currentUsername = "";
    currentUserId = -1;
    isLoggedIn = false;
    wrongPasswordAttempts = 0;
    forgotPasswordBtn->hide();
    showLogin();
}

void MainWindow::showLogin()
{
    stackedWidget->setCurrentWidget(loginWidget);
}

void MainWindow::showMainMenu()
{
    userLabel->setText("Welcome, " + currentUsername + "!");
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

void MainWindow::startClassicGame()
{
    GameWindow *gameWindow = new GameWindow("Classic Mode", this);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    stackedWidget->addWidget(gameWindow);
    stackedWidget->setCurrentWidget(gameWindow);

    connect(gameWindow, &GameWindow::backToMenuRequested, this, &MainWindow::showMainMenu);
}

void MainWindow::startOverwriteGame()
{
    GameWindow *gameWindow = new GameWindow("Overwrite Mode", this);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    stackedWidget->addWidget(gameWindow);
    stackedWidget->setCurrentWidget(gameWindow);

    connect(gameWindow, &GameWindow::backToMenuRequested, this, &MainWindow::showMainMenu);
}

void MainWindow::startEasyAI()
{
    GameWindow *gameWindow = new GameWindow("AI Easy", this);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    stackedWidget->addWidget(gameWindow);
    stackedWidget->setCurrentWidget(gameWindow);

    connect(gameWindow, &GameWindow::backToMenuRequested, this, &MainWindow::showMainMenu);
}

void MainWindow::startMediumAI()
{
    GameWindow *gameWindow = new GameWindow("AI Medium", this);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    stackedWidget->addWidget(gameWindow);
    stackedWidget->setCurrentWidget(gameWindow);

    connect(gameWindow, &GameWindow::backToMenuRequested, this, &MainWindow::showMainMenu);
}

void MainWindow::startHardAI()
{
    GameWindow *gameWindow = new GameWindow("AI Hard", this);
    gameWindow->setCurrentUser(currentUsername, currentUserId);

    stackedWidget->addWidget(gameWindow);
    stackedWidget->setCurrentWidget(gameWindow);

    connect(gameWindow, &GameWindow::backToMenuRequested, this, &MainWindow::showMainMenu);
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

    stackedWidget->addWidget(historyWindow);
    stackedWidget->setCurrentWidget(historyWindow);

    connect(historyWindow, &HistoryWindow::backToMainRequested, this, [=]() {
        stackedWidget->setCurrentWidget(mainMenuWidget);
        stackedWidget->removeWidget(historyWindow);
        historyWindow->deleteLater();
    });
}
