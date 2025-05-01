#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>

// Simple hash function (just for illustration, not a real security hash)
std::string simpleHash(const std::string& input) {
    std::hash<std::string> hasher;
    size_t hash = hasher(input);
    return std::to_string(hash);
}

// Simple database wrapper
class TicTacToeDB {
private:
    sqlite3* db;
    
    void executeSQL(const std::string& sql) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            throw std::runtime_error("Database error");
        }
    }

public:
    TicTacToeDB() {
        // Open database (creates if doesn't exist)
        if (sqlite3_open("tictactoe.db", &db) != SQLITE_OK) {
            throw std::runtime_error("Failed to open database");
        }
        
        // Create tables if they don't exist
        executeSQL("CREATE TABLE IF NOT EXISTS users ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "username TEXT UNIQUE NOT NULL, "
                  "password_hash TEXT NOT NULL);");
                  
        executeSQL("CREATE TABLE IF NOT EXISTS games ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "player1_id INTEGER NOT NULL, "
                  "player2_id INTEGER, "
                  "winner INTEGER, "
                  "moves TEXT, "  // Stores moves as "x,y,player;x,y,player;..."
                  "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
                  "FOREIGN KEY(player1_id) REFERENCES users(id), "
                  "FOREIGN KEY(player2_id) REFERENCES users(id));");
    }

    ~TicTacToeDB() {
        sqlite3_close(db);
    }

    // User management
    bool createUser(const std::string& username, const std::string& password) {
        sqlite3_stmt* stmt;
        std::string sql = "INSERT INTO users (username, password_hash) VALUES (?, ?)";
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        std::string hashed = simpleHash(password);
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Error executing SQL: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        return true;
    }

    bool validateUser(const std::string& username, const std::string& password) {
        sqlite3_stmt* stmt;
        std::string sql = "SELECT password_hash FROM users WHERE username = ?";
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }
        
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            // In real app, compare hashed passwords
            bool valid = (simpleHash(password) == storedHash); 
            sqlite3_finalize(stmt);
            return valid;
        }
        
        sqlite3_finalize(stmt);
        return false;
    }

    // Game history
    void saveGame(int player1Id, int player2Id, int winner, const std::vector<std::string>& moves) {
        sqlite3_stmt* stmt;
        std::string sql = "INSERT INTO games (player1_id, player2_id, winner, moves) VALUES (?, ?, ?, ?)";
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }
        
        // Combine moves into a single string (e.g., "0,0,1;1,1,2;...")
        std::string movesStr;
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
            throw std::runtime_error("Failed to save game");
        }
        
        sqlite3_finalize(stmt);
    }

    struct GameRecord {
        int id;
        int player1Id;
        int player2Id;
        int winner;
        std::string moves;
        std::string timestamp;
    };

    std::vector<GameRecord> getGameHistory(int userId) {
        std::vector<GameRecord> history;
        sqlite3_stmt* stmt;
        std::string sql = "SELECT id, player1_id, player2_id, winner, moves, timestamp "
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
        
        // Create some test users
        if (!db.createUser("player1", "pass123")) {
            std::cout << "Error creating user 'player1'\n";
            return 1;
        }
        if (!db.createUser("player2", "pass456")) {
            std::cout << "Error creating user 'player2'\n";
            return 1;
        }

        // Debug print the hash for player1 and player2 passwords
        std::cout << "Hashed password for 'player1': " << simpleHash("pass123") << std::endl;
        std::cout << "Hashed password for 'player2': " << simpleHash("pass456") << std::endl;

        // Validate user
        if (db.validateUser("player1", "pass123")) {
            std::cout << "Login successful!\n";

            // Simulate a game (player1 vs player2)
            std::vector<std::string> moves = {
                "0,0,1",  // Player1 marks top-left
                "1,1,2",   // Player2 marks center
                "0,1,1",  // Player1 marks top-middle
                "2,2,2",   // Player2 marks bottom-right
                "0,2,1"    // Player1 wins (top row)
            };

            // Save the game (player1 wins)
            db.saveGame(1, 2, 1, moves);

            // Get game history
            auto history = db.getGameHistory(1);
            std::cout << "Found " << history.size() << " games in history\n";
            if (!history.empty()) {
                std::cout << "Last game moves: " << history[0].moves << "\n";
            }
        } else {
            std::cout << "Invalid login\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
