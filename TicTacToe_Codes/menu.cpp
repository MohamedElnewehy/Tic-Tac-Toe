#include "menu.h"
#include "classic_game.h"
#include "overwrite_game.h"
#include "ai_engine.h"
#include <iostream>
using namespace std;

void showMenu() {
    cout << "\n================================\n";
    cout << "    TIC TAC TOE GAME MENU\n";
    cout << "================================\n";
    cout << "1. Classic Mode (Standard Rules)\n";
    cout << "2. Overwrite Mode (Can overwrite cells)\n";
    cout << "3. AI Mode (Play against AI)\n";
    cout << "4. Exit\n";
    cout << "================================\n";
    cout << "Choose an option (1-4): ";
}

void handleMenuChoice() {
    int choice;

    while (true) {
        showMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                startClassicGame();
                break;
            case 2:
                startOverwriteGame();
                break;
            case 3:
                startAIGame();
                break;
            case 4:
                cout << "Thanks for playing! Goodbye!\n";
                return;
            default:
                cout << "Invalid choice! Please select 1, 2, 3, or 4.\n";
                break;
        }

        cout << "\nPress Enter to return to menu...";
        cin.ignore();
        cin.get();
    }
}

