#include "MainWindow.h"
#include <QApplication>
#include <QFont>
#include <QScreen>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    applyModernStyling();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

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

void MainWindow::setupMainMenu()
{
    mainMenuWidget = new QWidget();
    mainLayout = new QVBoxLayout(mainMenuWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Header Section
    headerFrame = new QFrame();
    headerFrame->setFixedHeight(200);
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
    headerLayout->addWidget(subtitleLabel);

    // Button Section
    buttonFrame = new QFrame();
    buttonFrame->setStyleSheet("QFrame { background-color: #f5f7fa; }");

    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonFrame);
    buttonLayout->setSpacing(20);
    buttonLayout->setContentsMargins(40, 40, 40, 40);

    // Create styled buttons
    classicGameBtn = createStyledButton("Classic Mode", "#2ecc71");
    overwriteGameBtn = createStyledButton("Overwrite Mode", "#3498db");
    aiGameBtn = createStyledButton("Play vs AI", "#e67e22");
    historyBtn = createStyledButton("Game History", "#9b59b6");
    settingsBtn = createStyledButton("Settings", "#7f8c8d");
    exitBtn = createStyledButton("Exit", "#e74c3c");

    addButtonAnimation(classicGameBtn);
    addButtonAnimation(overwriteGameBtn);
    addButtonAnimation(aiGameBtn);
    addButtonAnimation(historyBtn);
    addButtonAnimation(settingsBtn);
    addButtonAnimation(exitBtn);

    buttonLayout->addWidget(classicGameBtn);
    buttonLayout->addWidget(overwriteGameBtn);
    buttonLayout->addWidget(aiGameBtn);
    buttonLayout->addWidget(historyBtn);
    buttonLayout->addWidget(settingsBtn);
    buttonLayout->addWidget(exitBtn);
    buttonLayout->addStretch();

    mainLayout->addWidget(headerFrame);
    mainLayout->addWidget(buttonFrame);

    // Connect signals
    connect(classicGameBtn, &QPushButton::clicked, this, &MainWindow::startClassicGame);
    connect(overwriteGameBtn, &QPushButton::clicked, this, &MainWindow::startOverwriteGame);
    connect(aiGameBtn, &QPushButton::clicked, this, &MainWindow::showAIMenu);
    connect(historyBtn, &QPushButton::clicked, this, &MainWindow::showGameHistory);
    connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::showSettings);
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
                                       color == "#f39c12" ? "#e67e22" : color)
                              .arg(color == "#2ecc71" ? "#229954" :
                                       color == "#3498db" ? "#21618c" :
                                       color == "#e67e22" ? "#ba4a00" :
                                       color == "#9b59b6" ? "#7d3c98" :
                                       color == "#7f8c8d" ? "#5d6d7e" :
                                       color == "#e74c3c" ? "#a93226" :
                                       color == "#f39c12" ? "#d68910" : color);

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

// Slot implementations
void MainWindow::showMainMenu()
{
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

void MainWindow::startClassicGame()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("Classic Mode", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::startOverwriteGame()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("Overwrite Mode", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::startEasyAI()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("AI Easy", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::startMediumAI()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("AI Medium", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::startHardAI()
{
    this->hide();

    GameWindow *gameWindow = new GameWindow("AI Hard", nullptr);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(gameWindow, &QWidget::destroyed, this, &QWidget::show);

    gameWindow->show();
}

void MainWindow::showAIMenu()
{
    stackedWidget->setCurrentWidget(aiMenuWidget);
}

void MainWindow::showGameHistory()
{
    QMessageBox::information(this, "Game History", "Game History feature will be implemented later.");
}

void MainWindow::showSettings()
{
    QMessageBox::information(this, "Settings", "Settings feature will be implemented later.");
}
