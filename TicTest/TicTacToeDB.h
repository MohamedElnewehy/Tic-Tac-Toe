#ifndef TICTACTOEDB_H
#define TICTACTOEDB_H

#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>
#include "picosha2.h"

using namespace std;

// Secure SHA-256 hash function using PicoSHA2
string sha256Hash(const string& input);

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
    TicTacToeDB();
    ~TicTacToeDB();

    // User Management (Login functionality)
    bool createUser(const string& username, const string& password);
    bool validateUser(const string& username, const string& password);
    bool deleteUser(const string& username);
    int getUserId(const string& username);
    bool userExists(const string& username);
    string getUsernameById(int userId);

    // Game Management (History functionality)
    void saveGame(int player1Id, int player2Id, int winner, const vector<string>& moves, const string& gameMode = "Classic");
    bool deleteAllGamesForUser(int userId);
    bool deleteGame(int gameId);

    struct GameRecord {
        int id;
        int player1Id;
        int player2Id;
        int winner;
        string moves;
        string timestamp;
        string player1Name;
        string player2Name;
        string gameMode;
    };

    struct UserStats {
        int totalGames;
        int wins;
        int losses;
        int draws;
        double winRate;
    };

    vector<GameRecord> getGameHistory(int userId);
    UserStats getUserStats(int userId);
};

#endif // TICTACTOEDB_H
