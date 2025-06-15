#include <iostream>
#include <string>
#include "sqlite/sqlite3.h"
#include <vector>
#include "picosha2.h" 
#include "gtest/gtest.h"

using namespace std;

// Secure SHA-256 hash function using PicoSHA2
string sha256Hash(const string& input) {
    return picosha2::hash256_hex_string(input);
}

// Simple database wrapper
class TicTacToeDB {
private:
    sqlite3* db;

    void executeSQL(const string& sql) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
            throw runtime_error("Database error");
        }
    }

public:
    TicTacToeDB() {
        if (sqlite3_open("tictactoe.db", &db) != SQLITE_OK) {
            throw runtime_error("Failed to open database");
        }

        // Enable foreign key support
        executeSQL("PRAGMA foreign_keys = ON;");

        // Drop tables if they already exist (to ensure ON DELETE CASCADE takes effect)
        executeSQL("DROP TABLE IF EXISTS games;");
        executeSQL("DROP TABLE IF EXISTS users;");

        executeSQL("CREATE TABLE IF NOT EXISTS users ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "username TEXT UNIQUE NOT NULL, "
                   "password_hash TEXT NOT NULL);");

        executeSQL("CREATE TABLE IF NOT EXISTS games ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "player1_id INTEGER NOT NULL, "
                   "player2_id INTEGER, "
                   "winner INTEGER, "
                   "moves TEXT, "
                   "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
                   "FOREIGN KEY(player1_id) REFERENCES users(id) ON DELETE CASCADE, "
                   "FOREIGN KEY(player2_id) REFERENCES users(id) ON DELETE CASCADE);");
    }
/*destructor, doesn't need testing*/
    ~TicTacToeDB() {
        sqlite3_close(db);
    }
/*testing that if I allocate a user into the database it will be stored*/
    bool createUser(const string& username, const string& password) {
        sqlite3_stmt* stmt;

        string checkSql = "SELECT id FROM users WHERE username = ?";
        if (sqlite3_prepare_v2(db, checkSql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare check statement\n";
            return false;
        }
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sqlite3_finalize(stmt);
            cerr << "Username already exists\n";
            return false;
        }
        sqlite3_finalize(stmt);

        string insertSql = "INSERT INTO users (username, password_hash) VALUES (?, ?)";
        if (sqlite3_prepare_v2(db, insertSql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to prepare insert statement\n";
            return false;
        }

        string hashed = sha256Hash(password);
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, SQLITE_TRANSIENT);

        bool result = sqlite3_step(stmt) == SQLITE_DONE;
        if (!result) {
            cerr << "Error creating user '" << username << "'\n";
        }

        sqlite3_finalize(stmt);
        return result;
    }

    bool validateUser(const string& username, const string& password) {
        sqlite3_stmt* stmt;
        string sql = "SELECT password_hash FROM users WHERE username = ?";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            bool valid = (sha256Hash(password) == storedHash);
            sqlite3_finalize(stmt);
            return valid;
        }

        sqlite3_finalize(stmt);
        return false;
    }

    bool deleteUser(const string& username) {
    sqlite3_stmt* stmt;
    string sql = "DELETE FROM users WHERE username = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare delete statement\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Failed to delete user\n";
        sqlite3_finalize(stmt);
        return false;
    }

    int rowsDeleted = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    if (rowsDeleted == 0) {
        // No user with that username was found
        return false;
    }

    // User was deleted successfully
    return true;
}

    void saveGame(int player1Id, int player2Id, int winner, const vector<string>& moves) {
        sqlite3_stmt* stmt;
        string sql = "INSERT INTO games (player1_id, player2_id, winner, moves) VALUES (?, ?, ?, ?)";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw runtime_error("Failed to prepare statement");
        }

        string movesStr;
        for (const auto& move : moves) {
            if (!movesStr.empty()) movesStr += ";";
            movesStr += move;
        }

        sqlite3_bind_int(stmt, 1, player1Id);
        player2Id == -1 ? sqlite3_bind_null(stmt, 2) : sqlite3_bind_int(stmt, 2, player2Id);
        winner == -1 ? sqlite3_bind_null(stmt, 3) : sqlite3_bind_int(stmt, 3, winner);
        sqlite3_bind_text(stmt, 4, movesStr.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw runtime_error("Failed to save game");
        }

        sqlite3_finalize(stmt);
    }

    struct GameRecord {
        int id;
        int player1Id;
        int player2Id;
        int winner;
        string moves;
        string timestamp;
    };

    vector<GameRecord> getGameHistory(int userId) {
        vector<GameRecord> history;
        sqlite3_stmt* stmt;
        string sql = "SELECT id, player1_id, player2_id, winner, moves, timestamp "
                     "FROM games WHERE player1_id = ? OR player2_id = ? "
                     "ORDER BY timestamp DESC LIMIT 10";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return history;
        }

        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_int(stmt, 2, userId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            GameRecord record;
            record.id = sqlite3_column_int(stmt, 0);
            record.player1Id = sqlite3_column_int(stmt, 1);
            record.player2Id = sqlite3_column_int(stmt, 2);
            record.winner = sqlite3_column_type(stmt, 3) == SQLITE_NULL ?
                            -1 : sqlite3_column_int(stmt, 3);
            record.moves = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            record.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            history.push_back(record);
        }

        sqlite3_finalize(stmt);
        return history;
    }
};
TEST(DB,user)//here I am testing a full user database
{
	TicTacToeDB TEST_DB;

EXPECT_TRUE(TEST_DB.createUser("ahmed_shousha","ahmed2004"));
EXPECT_TRUE(TEST_DB.validateUser("ahmed_shousha","ahmed2004"));
/*testing that the username/password pair is checked for correctly*/
EXPECT_FALSE(TEST_DB.validateUser("ahmed_shousha","anyWrongPassword"));
EXPECT_FALSE(TEST_DB.validateUser("anyWrongUsername","ahmed2004"));
/*testing unregistered users*/
EXPECT_FALSE(TEST_DB.validateUser("anyWrongUsername","anyWrongPassword"));
/*just clearing the database*/
TEST_DB.deleteUser("ahmed_shousha");
}


TEST(TEST_DB,delete_user)//checking if deletion is executed correctly
{
	TicTacToeDB TEST_DB_delete;

TEST_DB_delete.createUser("ahmedshousha","password");
//checking if each username-password pair is checked for correctly
EXPECT_FALSE(TEST_DB_delete.deleteUser("anyWrongUsername"));
/*here it should delete the user*/
EXPECT_TRUE(TEST_DB_delete.deleteUser("ahmedshousha"));//will return true if the user is deleted
/*checking that the user was actually deleted*/
EXPECT_FALSE(TEST_DB_delete.validateUser("ahmedshousha","password"));
}
TEST(TEST_DB,savedgame)//here I am testing if the game is being saved correctly

{//games to be saved
 TicTacToeDB TEST_DB_games;
 TEST_DB_games.createUser("ahmed","2004");
 TEST_DB_games.createUser("nth","another");
 TEST_DB_games.createUser("another","anotheragain");
 string games[10] = {"XXXXXXXXX","OOOOOOOOO","XOOXXOOOO","XXOOXXOOO","XXOXXOXXO","OOXOOXOOX","XXXXOOOOO","XOXOXOXOO"};
 vector<string> gameMoves;

 for(int i=0;i<8;i++){
gameMoves.push_back(string(games[i]));
 }
//arbitrary IDs
vector<TicTacToeDB::GameRecord> recordvector;
TEST_DB_games.saveGame(1,2,1,{gameMoves[0]});
TEST_DB_games.saveGame(1,2,1,{gameMoves[1]});
TEST_DB_games.saveGame(1,2,2,{gameMoves[2]});
TEST_DB_games.saveGame(1,2,2,{gameMoves[3]});
TEST_DB_games.saveGame(1,3,1,{gameMoves[4]});
TEST_DB_games.saveGame(1,3,1,{gameMoves[5]});
TEST_DB_games.saveGame(1,3,1,{gameMoves[6]});
TEST_DB_games.saveGame(1,3,3,{gameMoves[7]});
recordvector = TEST_DB_games.getGameHistory(1);
/*testing that player1 was correctly stored*/
EXPECT_EQ(recordvector[0].player1Id,1);
EXPECT_EQ(recordvector[1].player1Id,1);
EXPECT_EQ(recordvector[2].player1Id,1);
EXPECT_EQ(recordvector[3].player1Id,1);
EXPECT_EQ(recordvector[4].player1Id,1);
EXPECT_EQ(recordvector[5].player1Id,1);
EXPECT_EQ(recordvector[6].player1Id,1);
EXPECT_EQ(recordvector[7].player1Id,1);
/*testing that player2 was correctly stored*/
EXPECT_EQ(recordvector[0].player2Id,2);
EXPECT_EQ(recordvector[1].player2Id,2);
EXPECT_EQ(recordvector[2].player2Id,2);
EXPECT_EQ(recordvector[3].player2Id,2);
EXPECT_EQ(recordvector[4].player2Id,3);
EXPECT_EQ(recordvector[5].player2Id,3);
EXPECT_EQ(recordvector[6].player2Id,3);
EXPECT_EQ(recordvector[7].player2Id,3);
/*checking that the winner was stored correctly*/
EXPECT_EQ(recordvector[0].winner,1);
EXPECT_EQ(recordvector[1].winner,1);
EXPECT_EQ(recordvector[2].winner,2);
EXPECT_EQ(recordvector[3].winner,2);
EXPECT_EQ(recordvector[4].winner,1);
EXPECT_EQ(recordvector[5].winner,1);
EXPECT_EQ(recordvector[6].winner,1);
EXPECT_EQ(recordvector[7].winner,3);
/*checking that the sequence was stored correctly*/
EXPECT_EQ(recordvector[0].moves,"XXXXXXXXX");
EXPECT_EQ(recordvector[1].moves,"OOOOOOOOO");
EXPECT_EQ(recordvector[2].moves,"XOOXXOOOO");
EXPECT_EQ(recordvector[3].moves,"XXOOXXOOO");
EXPECT_EQ(recordvector[4].moves,"XXOXXOXXO");
EXPECT_EQ(recordvector[5].moves,"OOXOOXOOX");
EXPECT_EQ(recordvector[6].moves,"XXXXOOOOO");
EXPECT_EQ(recordvector[7].moves,"XOXOXOXOO");

}
/*end of the testing part*/
int main(int argc, char** argv) {

::testing::InitGoogleTest(&argc,argv);
return RUN_ALL_TESTS();
 
}
