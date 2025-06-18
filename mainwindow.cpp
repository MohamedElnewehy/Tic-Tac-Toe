#include "MainWindow.h"
#include <QApplication>
#include <QFont>
#include <QScreen>
#include <QGuiApplication>
#include <QFormLayout>


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

    // ✅ Let the window be resizable and include standard OS window buttons
    resize(1000, 800);  // Or use another default starting size you prefer

    // Optional: center on screen (you can keep this)
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
    loginHeaderFrame->setFixedHeight(220);
    loginHeaderFrame->setStyleSheet(
        "QFrame {"
        "background-color: #4a90e2;"
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

    // Login Buttons Frame
    QFrame *loginButtonFrame = new QFrame();
    loginButtonFrame->setStyleSheet("QFrame { background-color: #f5f7fa; }");
    loginButtonFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *loginButtonLayout = new QVBoxLayout(loginButtonFrame);
    loginButtonLayout->setSpacing(20);
    loginButtonLayout->setContentsMargins(40, 40, 40, 40);

    // Create buttons
    loginBtn = createStyledButton("🔑 Login", "#3498db");
    registerBtn = createStyledButton("📝 Register", "#2ecc71");
    guestBtn = createStyledButton("👤 Continue as Guest", "#95a5a6");

    // Forgot password button (initially hidden)
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

    // Connect signals
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

    // Create styled buttons
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

    // Connect signals
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
    // Create custom login dialog
    QDialog loginDialog(this);
    loginDialog.setWindowTitle("Login");
    loginDialog.setFixedSize(350, 280);
    loginDialog.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    loginDialog.setStyleSheet("QDialog { background-color: #f8f9fa; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(&loginDialog);

    // Title
    QLabel* titleLabel = new QLabel("Login");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #495057; margin: 10px;");

    // Username input
    QLabel* usernameLabel = new QLabel("Enter username:");
    usernameLabel->setStyleSheet("font-weight: bold; color: #6c757d;");
    QLineEdit* usernameEdit = new QLineEdit();
    usernameEdit->setStyleSheet(
        "QLineEdit {"
        "    padding: 8px;"
        "    border: 2px solid #e9ecef;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    background-color: white;"
        "    color: #495057;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #007bff;"
        "}"
        );

    // Password input
    QLabel* passwordLabel = new QLabel("Enter password:");
    passwordLabel->setStyleSheet("font-weight: bold; color: #6c757d;");
    QLineEdit* passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setStyleSheet(usernameEdit->styleSheet());

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* okBtn = new QPushButton("OK");
    QPushButton* cancelBtn = new QPushButton("Cancel");

    okBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #007bff;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 16px;"
        "    border-radius: 4px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0056b3;"
        "}"
        );

    cancelBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #6c757d;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 16px;"
        "    border-radius: 4px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #545b62;"
        "}"
        );

    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okBtn);

    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(usernameLabel);
    mainLayout->addWidget(usernameEdit);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    connect(okBtn, &QPushButton::clicked, &loginDialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &loginDialog, &QDialog::reject);
    connect(usernameEdit, &QLineEdit::returnPressed, [passwordEdit]() {
        passwordEdit->setFocus();
    });
    connect(passwordEdit, &QLineEdit::returnPressed, &loginDialog, &QDialog::accept);

    usernameEdit->setFocus();

    // Your original logic
    bool ok = (loginDialog.exec() == QDialog::Accepted);
    QString username = usernameEdit->text();
    if (!ok || username.isEmpty()) return false;

    QString password = passwordEdit->text();
    if (!ok || password.isEmpty()) return false;

    try {
        if (database->validateUser(username.toStdString(), password.toStdString())) {
            currentUsername = username;
            currentUserId = database->getUserId(username.toStdString());
            isLoggedIn = true;
            wrongPasswordAttempts = 0;
            forgotPasswordBtn->hide();

            // Save session
            database->saveUserSession(username.toStdString());

            showMainMenu();
            return true;
        } else {
            wrongPasswordAttempts++;
            if (wrongPasswordAttempts >= 1) {
                forgotPasswordBtn->show();
            }

            // Light styled warning message
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Login Failed");
            msgBox.setText("Invalid username or password.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet(
                "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
                "QLabel { color: black; font-size: 14px; }"
                "QPushButton {"
                "    background-color: #dc3545;"
                "    color: white;"
                "    border: none;"
                "    padding: 8px 16px;"
                "    border-radius: 4px;"
                "    font-weight: bold;"
                "    min-width: 80px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #c82333;"
                "}"
                );


            msgBox.exec();
            return false;
        }
    } catch (const std::exception& e) {
        // Light styled critical message
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Error");
        msgBox.setText(e.what());
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
            "QLabel { color: black; font-size: 14px; }"
            "QPushButton {"
            "    background-color: #dc3545;"
            "    color: white;"
            "    border: none;"
            "    padding: 8px 16px;"
            "    border-radius: 4px;"
            "    font-weight: bold;"
            "    min-width: 80px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #c82333;"
            "}"
            );

        msgBox.exec();
        return false;
    }
}

bool MainWindow::registerUser()
{
    // Create dialog with improved styling
    QDialog dialog(this);
    dialog.setWindowTitle("Create New Account");
    dialog.setFixedSize(450, 420);
    dialog.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    dialog.setStyleSheet("QDialog { background-color: #f8f9fa; }");

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    // Title
    QLabel* titleLabel = new QLabel("Create New Account");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #495057; margin: 15px;");

    // Input fields
    QLineEdit* usernameEdit = new QLineEdit;
    QLineEdit* passwordEdit = new QLineEdit;
    QLineEdit* colorEdit = new QLineEdit;
    QLineEdit* mealEdit = new QLineEdit;
    QLineEdit* wordEdit = new QLineEdit;
    QLineEdit* birthPlaceEdit = new QLineEdit;

    passwordEdit->setEchoMode(QLineEdit::Password);

    // Add placeholders for better UX
    usernameEdit->setPlaceholderText("Enter your username");
    passwordEdit->setPlaceholderText("Enter password (min 4 characters)");
    colorEdit->setPlaceholderText("e.g., Blue, Red, Green");
    mealEdit->setPlaceholderText("e.g., Pizza, Sushi, Pasta");
    wordEdit->setPlaceholderText("Enter your favorite word");
    birthPlaceEdit->setPlaceholderText("e.g., New York, London");

    // Enhanced input styling
    QList<QLineEdit*> inputs = {usernameEdit, passwordEdit, colorEdit, mealEdit, wordEdit, birthPlaceEdit};
    for (QLineEdit* edit : inputs) {
        edit->setMinimumHeight(35);
        edit->setStyleSheet(
            "QLineEdit {"
            "    padding: 8px;"
            "    border: 2px solid #e9ecef;"
            "    border-radius: 5px;"
            "    font-size: 14px;"
            "    background-color: white;"
            "    color: #495057;"
            "}"
            "QLineEdit:focus {"
            "    border-color: #007bff;"
            "}"
            "QLineEdit::placeholder {"
            "    color: #adb5bd;"
            "}"
            );
    }

    // Form layout with better styling
    QFormLayout* formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    formLayout->setHorizontalSpacing(15);
    formLayout->setVerticalSpacing(12);
    formLayout->setContentsMargins(30, 10, 30, 20);

    // Style labels
    QStringList labelTexts = {"Username:", "Password:", "Favorite Color:", "Favorite Meal:", "Favorite Word:", "Birth Place:"};
    QList<QLabel*> labels;

    for (int i = 0; i < inputs.size(); ++i) {
        QLabel* label = new QLabel(labelTexts[i]);
        label->setStyleSheet("font-weight: bold; color: #6c757d; font-size: 14px;");
        labels.append(label);
        formLayout->addRow(label, inputs[i]);
    }

    // Enhanced buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // Style OK button
    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText("Create Account");
    okButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #28a745;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #218838;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1e7e34;"
        "}"
        );

    // Style Cancel button
    QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #6c757d;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #545b62;"
        "}"
        );

    // Add to main layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    mainLayout->addStretch();

    // Connect buttons
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Add Enter key navigation
    connect(usernameEdit, &QLineEdit::returnPressed, [passwordEdit]() { passwordEdit->setFocus(); });
    connect(passwordEdit, &QLineEdit::returnPressed, [colorEdit]() { colorEdit->setFocus(); });
    connect(colorEdit, &QLineEdit::returnPressed, [mealEdit]() { mealEdit->setFocus(); });
    connect(mealEdit, &QLineEdit::returnPressed, [wordEdit]() { wordEdit->setFocus(); });
    connect(wordEdit, &QLineEdit::returnPressed, [birthPlaceEdit]() { birthPlaceEdit->setFocus(); });
    connect(birthPlaceEdit, &QLineEdit::returnPressed, &dialog, &QDialog::accept);

    // Set initial focus
    usernameEdit->setFocus();

    if (dialog.exec() == QDialog::Accepted) {
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text();
        QString favoriteColor = colorEdit->text().trimmed();
        QString favoriteMeal = mealEdit->text().trimmed();
        QString favoriteWord = wordEdit->text().trimmed();
        QString birthPlace = birthPlaceEdit->text().trimmed();

        // Validation with styled message boxes
        if (username.isEmpty() || password.length() < 4 ||
            favoriteColor.isEmpty() || favoriteMeal.isEmpty() ||
            favoriteWord.isEmpty() || birthPlace.isEmpty()) {

            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Invalid Input");
            msgBox.setText("Please complete all fields.\nPassword must be at least 4 characters.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet(
                "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
                "QLabel { color: black; font-size: 14px; }"
                "QPushButton {"
                "    background-color: #dc3545;"
                "    color: white;"
                "    border: none;"
                "    padding: 8px 16px;"
                "    border-radius: 4px;"
                "    font-weight: bold;"
                "    min-width: 80px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #c82333;"
                "}"
                );

            msgBox.exec();
            return false;
        }

        if (database->userExists(username.toStdString())) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Registration Failed");
            msgBox.setText("Username is already taken.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet(
                "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
                "QLabel { color: black; font-size: 14px; }"
                "QPushButton {"
                "    background-color: #dc3545;"
                "    color: white;"
                "    border: none;"
                "    padding: 8px 16px;"
                "    border-radius: 4px;"
                "    font-weight: bold;"
                "    min-width: 80px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #c82333;"
                "}"
                );

            msgBox.exec();
            return false;
        }

        try {
            if (database->createUser(username.toStdString(), password.toStdString(),
                                     favoriteColor.toStdString(), favoriteMeal.toStdString(),
                                     favoriteWord.toStdString(), birthPlace.toStdString())) {

                QMessageBox msgBox(this);
                msgBox.setWindowTitle("Success");
                msgBox.setText("Account created successfully!");
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setStyleSheet(
                    "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
                    "QLabel { color: black; font-size: 14px; }"
                    "QPushButton {"
                    "    background-color: #dc3545;"
                    "    color: white;"
                    "    border: none;"
                    "    padding: 8px 16px;"
                    "    border-radius: 4px;"
                    "    font-weight: bold;"
                    "    min-width: 80px;"
                    "}"
                    "QPushButton:hover {"
                    "    background-color: #c82333;"
                    "}"
                    );

                msgBox.exec();

                currentUsername = username;
                currentUserId = database->getUserId(username.toStdString());
                isLoggedIn = true;
                database->saveUserSession(username.toStdString());
                showMainMenu();
                return true;
            } else {
                QMessageBox msgBox(this);
                msgBox.setWindowTitle("Error");
                msgBox.setText("Failed to create account.");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setStyleSheet(
                    "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
                    "QLabel { color: black; font-size: 14px; }"
                    "QPushButton {"
                    "    background-color: #dc3545;"
                    "    color: white;"
                    "    border: none;"
                    "    padding: 8px 16px;"
                    "    border-radius: 4px;"
                    "    font-weight: bold;"
                    "    min-width: 80px;"
                    "}"
                    "QPushButton:hover {"
                    "    background-color: #c82333;"
                    "}"
                    );

                msgBox.exec();
                return false;
            }
        } catch (const std::exception& e) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Error");
            msgBox.setText(e.what());
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setStyleSheet(
                "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
                "QLabel { color: black; font-size: 14px; }"
                "QPushButton {"
                "    background-color: #dc3545;"
                "    color: white;"
                "    border: none;"
                "    padding: 8px 16px;"
                "    border-radius: 4px;"
                "    font-weight: bold;"
                "    min-width: 80px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #c82333;"
                "}"
                );

            msgBox.exec();
            return false;
        }
    }

    return false;
}



void MainWindow::forgotPassword()
{
    // Create single dialog with all fields like register
    QDialog dialog(this);
    dialog.setWindowTitle("Forgot Password");
    dialog.setFixedSize(450, 420);
    dialog.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    dialog.setStyleSheet("QDialog { background-color: #f8f9fa; }");

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    // Title
    QLabel* titleLabel = new QLabel("Reset Password");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #495057; margin: 15px;");

    // Input fields
    QLineEdit* usernameEdit = new QLineEdit;
    QLineEdit* colorEdit = new QLineEdit;
    QLineEdit* mealEdit = new QLineEdit;
    QLineEdit* wordEdit = new QLineEdit;
    QLineEdit* birthPlaceEdit = new QLineEdit;
    QLineEdit* newPasswordEdit = new QLineEdit;

    newPasswordEdit->setEchoMode(QLineEdit::Password);

    // Add placeholders for better UX
    usernameEdit->setPlaceholderText("Enter your username");
    colorEdit->setPlaceholderText("Enter your favorite color");
    mealEdit->setPlaceholderText("Enter your favorite meal");
    wordEdit->setPlaceholderText("Enter your favorite word");
    birthPlaceEdit->setPlaceholderText("Enter your birth place");
    newPasswordEdit->setPlaceholderText("Enter new password (min 4 characters)");

    // Enhanced input styling
    QList<QLineEdit*> inputs = {usernameEdit, colorEdit, mealEdit, wordEdit, birthPlaceEdit, newPasswordEdit};
    for (QLineEdit* edit : inputs) {
        edit->setMinimumHeight(35);
        edit->setStyleSheet(
            "QLineEdit {"
            "    padding: 8px;"
            "    border: 2px solid #e9ecef;"
            "    border-radius: 5px;"
            "    font-size: 14px;"
            "    background-color: white;"
            "    color: #495057;"
            "}"
            "QLineEdit:focus {"
            "    border-color: #007bff;"
            "}"
            "QLineEdit::placeholder {"
            "    color: #adb5bd;"
            "}"
            );
    }

    // Form layout with better styling
    QFormLayout* formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    formLayout->setHorizontalSpacing(15);
    formLayout->setVerticalSpacing(12);
    formLayout->setContentsMargins(30, 10, 30, 20);

    // Style labels
    QStringList labelTexts = {"Username:", "Favorite Color:", "Favorite Meal:", "Favorite Word:", "Birth Place:", "New Password:"};
    QList<QLabel*> labels;

    for (int i = 0; i < inputs.size(); ++i) {
        QLabel* label = new QLabel(labelTexts[i]);
        label->setStyleSheet("font-weight: bold; color: #6c757d; font-size: 14px;");
        labels.append(label);
        formLayout->addRow(label, inputs[i]);
    }

    // Enhanced buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // Style OK button
    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText("Reset Password");
    okButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #28a745;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #218838;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1e7e34;"
        "}"
        );

    // Style Cancel button
    QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #6c757d;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 5px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    min-width: 120px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #545b62;"
        "}"
        );

    // Add to main layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    mainLayout->addStretch();

    // Connect buttons
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Add Enter key navigation
    connect(usernameEdit, &QLineEdit::returnPressed, [colorEdit]() { colorEdit->setFocus(); });
    connect(colorEdit, &QLineEdit::returnPressed, [mealEdit]() { mealEdit->setFocus(); });
    connect(mealEdit, &QLineEdit::returnPressed, [wordEdit]() { wordEdit->setFocus(); });
    connect(wordEdit, &QLineEdit::returnPressed, [birthPlaceEdit]() { birthPlaceEdit->setFocus(); });
    connect(birthPlaceEdit, &QLineEdit::returnPressed, [newPasswordEdit]() { newPasswordEdit->setFocus(); });
    connect(newPasswordEdit, &QLineEdit::returnPressed, &dialog, &QDialog::accept);

    // Set initial focus
    usernameEdit->setFocus();

    // Your original logic adapted for single window
    bool ok = (dialog.exec() == QDialog::Accepted);
    if (!ok) return;

    QString username = usernameEdit->text().trimmed();
    QString favoriteColor = colorEdit->text().trimmed();
    QString favoriteMeal = mealEdit->text().trimmed();
    QString favoriteWord = wordEdit->text().trimmed();
    QString birthPlace = birthPlaceEdit->text().trimmed();
    QString newPassword = newPasswordEdit->text();

    // Validation - check if all fields are filled
    if (username.isEmpty() || favoriteColor.isEmpty() || favoriteMeal.isEmpty() ||
        favoriteWord.isEmpty() || birthPlace.isEmpty() || newPassword.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Invalid Input");
        msgBox.setText("Please complete all fields.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
            "QLabel { color: black; font-size: 14px; }"
            "QPushButton {"
            "    background-color: #dc3545;"
            "    color: white;"
            "    border: none;"
            "    padding: 8px 16px;"
            "    border-radius: 4px;"
            "    font-weight: bold;"
            "    min-width: 80px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #c82333;"
            "}"
            );

        msgBox.exec();
        return;
    }

    if (!database->userExists(username.toStdString())) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("User Not Found");
        msgBox.setText("Username does not exist.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
            "QLabel { color: black; font-size: 14px; }"
            "QPushButton {"
            "    background-color: #dc3545;"
            "    color: white;"
            "    border: none;"
            "    padding: 8px 16px;"
            "    border-radius: 4px;"
            "    font-weight: bold;"
            "    min-width: 80px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #c82333;"
            "}"
            );

        msgBox.exec();
        return;
    }

    // Validate security questions (assuming you have a method that takes all answers)
    if (database->validateSecurityAnswers(username.toStdString(), favoriteColor.toStdString(),
                                          favoriteMeal.toStdString(), favoriteWord.toStdString(),
                                          birthPlace.toStdString())) {

        if (newPassword.length() < 4) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Invalid Password");
            msgBox.setText("Password must be at least 4 characters long.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet(
                "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
                "QLabel { color: black; font-size: 14px; }"
                "QPushButton {"
                "    background-color: #dc3545;"
                "    color: white;"
                "    border: none;"
                "    padding: 8px 16px;"
                "    border-radius: 4px;"
                "    font-weight: bold;"
                "    min-width: 80px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #c82333;"
                "}"
                );

            msgBox.exec();
            return;
        }

        if (database->updatePassword(username.toStdString(), newPassword.toStdString())) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Success");
            msgBox.setText("Password changed successfully! You are now logged in.");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStyleSheet(
                "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
                "QLabel { color: black; font-size: 14px; }"
                "QPushButton {"
                "    background-color: #dc3545;"
                "    color: white;"
                "    border: none;"
                "    padding: 8px 16px;"
                "    border-radius: 4px;"
                "    font-weight: bold;"
                "    min-width: 80px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #c82333;"
                "}"
                );

            msgBox.exec();

            currentUsername = username;
            currentUserId = database->getUserId(username.toStdString());
            isLoggedIn = true;
            forgotPasswordBtn->hide();
            wrongPasswordAttempts = 0;

            // Save session
            database->saveUserSession(username.toStdString());

            showMainMenu();
        } else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Error");
            msgBox.setText("Failed to update password.");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setStyleSheet(
                "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
                "QLabel { color: black; font-size: 14px; }"
                "QPushButton {"
                "    background-color: #dc3545;"
                "    color: white;"
                "    border: none;"
                "    padding: 8px 16px;"
                "    border-radius: 4px;"
                "    font-weight: bold;"
                "    min-width: 80px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #c82333;"
                "}"
                );

            msgBox.exec();
        }
    } else {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Security Questions Failed");
        msgBox.setText("Security answers do not match our records.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: #f8f9fa; padding: 20px; }"
            "QLabel { color: black; font-size: 14px; }"
            "QPushButton {"
            "    background-color: #dc3545;"
            "    color: white;"
            "    border: none;"
            "    padding: 8px 16px;"
            "    border-radius: 4px;"
            "    font-weight: bold;"
            "    min-width: 80px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #c82333;"
            "}"
            );

        msgBox.exec();
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

// Slot implementations
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

    // Add to stacked widget and switch to it
    stackedWidget->addWidget(gameWindow);
    stackedWidget->setCurrentWidget(gameWindow);

    // Connect back signal
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

    // Handle back signal cleanly
    connect(historyWindow, &HistoryWindow::backToMainRequested, this, [=]() {
        stackedWidget->setCurrentWidget(mainMenuWidget);
        stackedWidget->removeWidget(historyWindow);
        historyWindow->deleteLater();
    });
}
