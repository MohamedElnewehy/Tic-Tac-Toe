#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>
#include "picosha2.h" 

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

    ~TicTacToeDB() {
        sqlite3_close(db);
    }

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

        bool success = sqlite3_step(stmt) == SQLITE_DONE;

        if (!success) {
            cerr << "Failed to delete user or user not found\n";
        }

        sqlite3_finalize(stmt);
        return success;
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

// Example usage
int main() {
    try {
        TicTacToeDB db;

        db.createUser("player1", "pass123");
        db.createUser("player2", "pass456");

        if (db.validateUser("player1", "pass123")) {
            cout << "Login successful!\n";

            vector<string> moves = {
                "0,0,1", "1,1,2", "0,1,1", "2,2,2", "0,2,1"
            };

            db.saveGame(1, 2, 1, moves);

            auto history = db.getGameHistory(1);
            cout << "Found " << history.size() << " games in history\n";
            if (!history.empty()) {
                cout << "Last game moves: " << history[0].moves << "\n";
            }

            // Try deleting player2
            if (db.deleteUser("player2")) {
                cout << "User 'player2' and their games were deleted successfully.\n";
            } else {
                cout << "Failed to delete user.\n";
            }

        } else {
            cout << "Invalid login\n";
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
