#include <iostream>
#include <sqlite3.h>

int main() {
    std::cout << "SQLite version: " << sqlite3_libversion() << std::endl;
    
    sqlite3* db;
    int rc = sqlite3_open("test.db", &db);
    
    if (rc) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    } else {
        std::cout << "Database opened successfully!" << std::endl;
    }
    
    // Close the database
    sqlite3_close(db);
    
    return 0;
}