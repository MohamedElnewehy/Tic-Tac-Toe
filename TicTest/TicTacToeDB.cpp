#include "TicTacToeDB.h"

// Secure SHA-256 hash function using PicoSHA2
string sha256Hash(const string& input) {
    return picosha2::hash256_hex_string(input);
}

TicTacToeDB::TicTacToeDB() {
    if (sqlite3_open("tictactoe.db", &db) != SQLITE_OK) {
        throw runtime_error("Failed to open database");
    }

    // Enable foreign key support
    executeSQL("PRAGMA foreign_keys = ON;");

    // Create users table for login functionality
    executeSQL("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT UNIQUE NOT NULL, "
               "password_hash TEXT NOT NULL, "
               "created_at DATETIME DEFAULT CURRENT_TIMESTAMP);");

    // Create games table for history functionality
    executeSQL("CREATE TABLE IF NOT EXISTS games ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "player1_id INTEGER NOT NULL, "
               "player2_id INTEGER, "
               "winner INTEGER, "
               "moves TEXT, "
               "game_mode TEXT DEFAULT 'Classic', "
               "game_duration INTEGER DEFAULT 0, "
               "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
               "FOREIGN KEY(player1_id) REFERENCES users(id) ON DELETE CASCADE, "
               "FOREIGN KEY(player2_id) REFERENCES users(id) ON DELETE CASCADE);");
}

TicTacToeDB::~TicTacToeDB() {
    sqlite3_close(db);
}

// LOGIN FUNCTIONALITY

bool TicTacToeDB::createUser(const string& username, const string& password) {
    // Input validation
    if (username.empty() || password.empty()) {
        cerr << "Username and password cannot be empty\n";
        return false;
    }

    if (password.length() < 4) {
        cerr << "Password must be at least 4 characters long\n";
        return false;
    }

    // Check if username already exists
    if (userExists(username)) {
        cerr << "Username already exists: " << username << "\n";
        return false;
    }

    sqlite3_stmt* stmt;
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

bool TicTacToeDB::validateUser(const string& username, const string& password) {
    if (username.empty() || password.empty()) {
        return false;
    }

    sqlite3_stmt* stmt;
    string sql = "SELECT password_hash FROM users WHERE username = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool isValid = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        string storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        isValid = (sha256Hash(password) == storedHash);
    }

    sqlite3_finalize(stmt);
    return isValid;
}

bool TicTacToeDB::userExists(const string& username) {
    sqlite3_stmt* stmt;
    string checkSql = "SELECT id FROM users WHERE username = ?";

    if (sqlite3_prepare_v2(db, checkSql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    bool exists = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);

    return exists;
}

int TicTacToeDB::getUserId(const string& username) {
    sqlite3_stmt* stmt;
    string sql = "SELECT id FROM users WHERE username = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    int userId = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return userId;
}

string TicTacToeDB::getUsernameById(int userId) {
    sqlite3_stmt* stmt;
    string sql = "SELECT username FROM users WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return "Unknown";
    }

    sqlite3_bind_int(stmt, 1, userId);

    string username = "Unknown";
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return username;
}

bool TicTacToeDB::deleteUser(const string& username) {
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

// HISTORY FUNCTIONALITY

void TicTacToeDB::saveGame(int player1Id, int player2Id, int winner, const vector<string>& moves, const string& gameMode) {
    sqlite3_stmt* stmt;
    string sql = "INSERT INTO games (player1_id, player2_id, winner, moves, game_mode, game_duration) VALUES (?, ?, ?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error("Failed to prepare statement");
    }

    // Convert moves vector to string with comma separator
    string movesStr;
    for (size_t i = 0; i < moves.size(); i++) {
        if (i > 0) movesStr += ",";
        movesStr += moves[i];
    }

    sqlite3_bind_int(stmt, 1, player1Id);
    player2Id == -1 ? sqlite3_bind_null(stmt, 2) : sqlite3_bind_int(stmt, 2, player2Id);
    winner == 0 ? sqlite3_bind_null(stmt, 3) : sqlite3_bind_int(stmt, 3, winner);
    sqlite3_bind_text(stmt, 4, movesStr.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, gameMode.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, static_cast<int>(moves.size())); // Game duration as number of moves

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw runtime_error("Failed to save game");
    }

    sqlite3_finalize(stmt);
}

vector<TicTacToeDB::GameRecord> TicTacToeDB::getGameHistory(int userId) {
    vector<GameRecord> history;
    sqlite3_stmt* stmt;
    string sql = "SELECT g.id, g.player1_id, g.player2_id, g.winner, g.moves, g.timestamp, g.game_mode, "
                 "u1.username as player1_name, u2.username as player2_name "
                 "FROM games g "
                 "LEFT JOIN users u1 ON g.player1_id = u1.id "
                 "LEFT JOIN users u2 ON g.player2_id = u2.id "
                 "WHERE g.player1_id = ? OR g.player2_id = ? "
                 "ORDER BY g.timestamp DESC LIMIT 50";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return history;
    }

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, userId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        GameRecord record;
        record.id = sqlite3_column_int(stmt, 0);
        record.player1Id = sqlite3_column_int(stmt, 1);
        record.player2Id = sqlite3_column_type(stmt, 2) == SQLITE_NULL ?
                               -1 : sqlite3_column_int(stmt, 2);
        record.winner = sqlite3_column_type(stmt, 3) == SQLITE_NULL ?
                            0 : sqlite3_column_int(stmt, 3);
        record.moves = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        record.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        record.gameMode = sqlite3_column_type(stmt, 6) == SQLITE_NULL ?
                              "Classic" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        record.player1Name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        record.player2Name = sqlite3_column_type(stmt, 8) == SQLITE_NULL ?
                                 "AI" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));

        history.push_back(record);
    }

    sqlite3_finalize(stmt);
    return history;
}

TicTacToeDB::UserStats TicTacToeDB::getUserStats(int userId) {
    UserStats stats = {0, 0, 0, 0, 0.0};
    sqlite3_stmt* stmt;

    string sql = "SELECT COUNT(*) as total_games, "
                 "SUM(CASE WHEN winner = ? THEN 1 ELSE 0 END) as wins, "
                 "SUM(CASE WHEN winner != ? AND winner IS NOT NULL AND winner != 0 THEN 1 ELSE 0 END) as losses, "
                 "SUM(CASE WHEN winner IS NULL OR winner = 0 THEN 1 ELSE 0 END) as draws "
                 "FROM games WHERE player1_id = ? OR player2_id = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return stats;
    }

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_bind_int(stmt, 3, userId);
    sqlite3_bind_int(stmt, 4, userId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        stats.totalGames = sqlite3_column_int(stmt, 0);
        stats.wins = sqlite3_column_int(stmt, 1);
        stats.losses = sqlite3_column_int(stmt, 2);
        stats.draws = sqlite3_column_int(stmt, 3);

        if (stats.totalGames > 0) {
            stats.winRate = (double)stats.wins / stats.totalGames * 100.0;
        }
    }

    sqlite3_finalize(stmt);
    return stats;
}

bool TicTacToeDB::deleteGame(int gameId) {
    sqlite3_stmt* stmt;
    string sql = "DELETE FROM games WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare delete game statement\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, gameId);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

bool TicTacToeDB::deleteAllGamesForUser(int userId) {
    sqlite3_stmt* stmt;
    string sql = "DELETE FROM games WHERE player1_id = ? OR player2_id = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, userId);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    if (!success) {
        cerr << "Failed to delete games: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}
