#include <gtest/gtest.h>
#include "TicTacToeDB.h"
#include <filesystem>
#include <QApplication>

class TicTacToeDBTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        if (!QApplication::instance()) {
            int argc = 0;
            char** argv = nullptr;
            app = new QApplication(argc, argv);
        }
    }

    void SetUp() override {
        testDbFile = "test_tictactoe.db";
        if (std::filesystem::exists(testDbFile)) {
            std::filesystem::remove(testDbFile);
        }
        db = new TicTacToeDB();
    }

    void TearDown() override {
        delete db;
        if (std::filesystem::exists(testDbFile)) {
            std::filesystem::remove(testDbFile);
        }
    }

    static QApplication* app;
    TicTacToeDB* db;
    std::string testDbFile;
};

QApplication* TicTacToeDBTest::app = nullptr;

TEST_F(TicTacToeDBTest, CreateUserSuccessfully) {
    bool result = db->createUser("testuser", "testpass123", 
                                 "blue", "pizza", "hello", "testcity");
    EXPECT_TRUE(result);
    EXPECT_TRUE(db->userExists("testuser"));
}

TEST_F(TicTacToeDBTest, CreateUserFailsWithShortPassword) {
    bool result = db->createUser("testuser", "123", 
                                 "blue", "pizza", "hello", "testcity");
    EXPECT_FALSE(result);
    EXPECT_FALSE(db->userExists("testuser"));
}

TEST_F(TicTacToeDBTest, ValidateUserCredentials) {
    db->createUser("testuser", "testpass123", "blue", "pizza", "hello", "testcity");
    EXPECT_TRUE(db->validateUser("testuser", "testpass123"));
    EXPECT_FALSE(db->validateUser("testuser", "wrongpass"));
    EXPECT_FALSE(db->validateUser("nonexistent", "testpass123"));
}

TEST_F(TicTacToeDBTest, SecurityQuestionsWork) {
    db->createUser("testuser", "testpass123", "blue", "pizza", "hello", "testcity");
    
    EXPECT_TRUE(db->validateSecurityAnswers("testuser", "blue", "pizza", "hello", "testcity"));
    EXPECT_FALSE(db->validateSecurityAnswers("testuser", "red", "pizza", "hello", "testcity"));
}

TEST_F(TicTacToeDBTest, SaveAndRetrieveGame) {
    db->createUser("player1", "pass123", "blue", "pizza", "hello", "city1");
    int player1Id = db->getUserId("player1");
    
    std::vector<std::string> moves = {"X00", "O01", "X11", "O02", "X22"};
    db->saveGame(player1Id, -1, player1Id, moves, "Classic Mode");
    
    auto history = db->getGameHistory(player1Id);
    EXPECT_EQ(history.size(), 1);
    EXPECT_EQ(history[0].winner, player1Id);
    EXPECT_EQ(history[0].gameMode, "Classic Mode");
}

TEST_F(TicTacToeDBTest, GetUserStatsCorrectly) {
    db->createUser("player1", "pass123", "blue", "pizza", "hello", "city1");
    int player1Id = db->getUserId("player1");
    
    // Add some games
    std::vector<std::string> moves = {"X00", "O01", "X11"};
    db->saveGame(player1Id, -1, player1Id, moves, "AI Easy"); // Win
    db->saveGame(player1Id, -1, -1, moves, "AI Easy");        // Loss
    db->saveGame(player1Id, -1, 0, moves, "AI Easy");         // Draw
    
    auto stats = db->getUserStats(player1Id);
    EXPECT_EQ(stats.totalGames, 3);
    EXPECT_EQ(stats.wins, 1);
    EXPECT_EQ(stats.losses, 1);
    EXPECT_EQ(stats.draws, 1);
    EXPECT_NEAR(stats.winRate, 33.33, 0.1);
}
