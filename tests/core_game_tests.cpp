#include <iostream>
#include <vector>
#include <gtest/gtest.h>
using namespace std;

class TicTacToe {
private:
    vector< vector<char> > board;
    char currentPlayer;
    //declaring a test class as a friend
    //giving access to private members to test them
    FRIEND_TEST(GameLogicTests, CheckWinCondition);
    FRIEND_TEST(GameLogicTests, CheckDrawCondition);
    FRIEND_TEST(GameLogicTests, PlayerSwitching);
    FRIEND_TEST(GameLogicTests, MoveValidation);
    
public:
    TicTacToe() {
        board = vector< vector<char> >(3, vector<char>(3, ' '));
        currentPlayer = 'X';
    }

    void printBoard() {
        cout << "\n";
        for (int i = 0; i < 3; ++i) {
            cout << " ";
            for (int j = 0; j < 3; ++j) {
                cout << board[i][j];
                if (j < 2) cout << " | ";
            }
            cout << "\n";
            if (i < 2) cout << "---+---+---\n";
        }
        cout << "\n";
    }

    bool makeMove(int row, int col) {
        if (row < 0 || row > 2 || col < 0 || col > 2) {
            cout << "Invalid move! Try again.\n";
            return false;
        }

        if (board[row][col] != ' ') {
            cout << "Cell already occupied! Try again.\n";
            return false;
        }

        board[row][col] = currentPlayer;
        return true;
    }

    bool checkWin() {
        for (int i = 0; i < 3; ++i) {
            if ((board[i][0] == currentPlayer &&
                 board[i][1] == currentPlayer &&
                 board[i][2] == currentPlayer) ||
                (board[0][i] == currentPlayer &&
                 board[1][i] == currentPlayer &&
                 board[2][i] == currentPlayer)) {
                return true;
            }
        }

        if ((board[0][0] == currentPlayer &&
             board[1][1] == currentPlayer &&
             board[2][2] == currentPlayer) ||
            (board[0][2] == currentPlayer &&
             board[1][1] == currentPlayer &&
             board[2][0] == currentPlayer)) {
            return true;
        }

        return false;
    }

    bool checkDraw() {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (board[i][j] == ' ')
                    return false;
        if(!checkWin());
        return true;
    }

    void switchPlayer() {
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }

    void startGame() {
        cout << "Tic Tac Toe 1v1 Game!\n";
        printBoard();

        while (true) {
            int row, col;
            cout << "Player " << currentPlayer << ", enter row and column (0-2): ";
            cin >> row >> col;

            if (!makeMove(row, col)) continue;

            printBoard();

            if (checkWin()) {
                cout << "Player " << currentPlayer << " wins!\n";
                break;
            }

            if (checkDraw()) {
                cout << "Game ended in a draw!\n";
                break;
            }

            switchPlayer();
        }
    }
};

TEST(GameLogicTests, PlayerSwitching){
    TicTacToe game;
    //testing the change from X to O since the default value is X
    EXPECT_EQ(game.currentPlayer, 'X');//default value must be X
    game.switchPlayer();
    EXPECT_EQ(game.currentPlayer, 'O');//must be switched to O
    game.switchPlayer();
    EXPECT_EQ(game.currentPlayer, 'X');//switched to X again
}

//testing the winning condition function
TEST(GameLogicTests, CheckWinCondition){
    TicTacToe game;
    //setting a specific row
    for(int k=0; k<3; k++)
        game.board[0][k] = game.currentPlayer;
    //checking for the win on row 1
    EXPECT_EQ(game.checkWin(), true);
    
    for(int k=0; k<3; k++){
        game.board[0][k] = ' ';//resetting the board
        game.board[1][k] = game.currentPlayer;
    }
    //checks for the win on row 2
    EXPECT_EQ(game.checkWin(), true);
    
    for(int k=0; k<3; k++){
        game.board[1][k] = ' ';//resetting the board
        game.board[2][k] = game.currentPlayer;
    }
    //checks for the win on row 3
    EXPECT_EQ(game.checkWin(), true);

    for(int k=0; k<3; k++){
        game.board[2][k] = ' ';//resetting the board
        game.board[k][0] = game.currentPlayer;
    }
    //checks for the win on column 1
    EXPECT_EQ(game.checkWin(), true);

    for(int k=0; k<3; k++){
        game.board[k][0] = ' ';//resetting the board
        game.board[k][1] = game.currentPlayer;
    }
    //checks for the win on column 2
    EXPECT_EQ(game.checkWin(), true);

    for(int k=0; k<3; k++){
        game.board[k][1] = ' ';//resetting the board
        game.board[k][2] = game.currentPlayer;
    }
    //checks for the win on column 3
    EXPECT_EQ(game.checkWin(), true);

    for(int k=0; k<3; k++){
        game.board[k][2] = ' ';//resetting the board
        game.board[k][k] = game.currentPlayer;
    }
    //checks for the win on the diagonal
    EXPECT_EQ(game.checkWin(), true);

    for(int k=0; k<3; k++){
        game.board[k][k] = ' ';//resetting the board
        game.board[2-k][k] = game.currentPlayer;
    }
    //checks for the win on the other diagonal
    EXPECT_EQ(game.checkWin(), true);
    
    for(int k=0; k<3; k++)
        game.board[2-k][k] = ' ';//resetting the board
    //all win conditions have been tested now to check if it doesnt detect a win
    EXPECT_EQ(game.checkWin(), false);
    //the previous line passing means the board was completely emptied
    //now we test if it doesn't register a win on 2 consecutive symbols
    game.board[0][0] = 'X';
    game.board[0][1] = 'X';
    EXPECT_FALSE(game.checkWin());
}

//testing the checkdraw function
TEST(GameLogicTests, CheckDrawCondition){
    TicTacToe game;
    /*since not all draw cases can be written easily we will prove a general case
    will put a winning case for row,column,draw then will check if it
    isnt a draw*/
    for(int k=0; k<3; k++)
        game.board[0][k] = game.currentPlayer;
    EXPECT_EQ(game.checkDraw(), false);
    
    for(int k=0; k<3; k++) {
        game.board[0][k] = ' ';
        game.board[k][0] = game.currentPlayer;
    }
    EXPECT_EQ(game.checkDraw(), false);
    
    for(int k=0; k<3; k++) {
        game.board[k][0]=' ';
        game.board[k][k]=game.currentPlayer;
    }
    EXPECT_EQ(game.checkDraw(), false);
    
    for(int k=0; k<3; k++) {
        game.board[k][k]=' ';
    }
    EXPECT_EQ(game.checkDraw(), false);
    
    //putting in a draw case
    game.board[1][1]='X';
    game.board[0][2]='O';
    game.board[0][1]='X';
    game.board[2][1]='O';
    game.board[1][2]='X';
    game.board[1][0]='O';
    game.board[0][0]='X';
    game.board[2][2]='O';
    game.board[2][0]='X';
    EXPECT_EQ(game.checkDraw(), true);
    EXPECT_FALSE(game.checkWin());
}

//testing the makemove function
TEST(GameLogicTests, MoveValidation){
    TicTacToe game;
    //inputting a wrong row below the valid values
    EXPECT_EQ(game.makeMove(-1,0), false);
    //inputting a wrong row above the valid values
    EXPECT_EQ(game.makeMove(3,0), false);
    //inputting a wrong column above the valid values
    EXPECT_EQ(game.makeMove(0,3), false);
    //inputting a wrong column below the valid values
    EXPECT_EQ(game.makeMove(0,-1), false);
    //inputting correct rows and columns and checking that it puts moves 
    //in the correct place
    for(int k=0; k<3; k++){
        for(int l=0; l<3; l++){
            EXPECT_EQ(game.makeMove(k,l), true);
            //testing for making moves on an already occupied position
            EXPECT_FALSE(game.makeMove(k,l));
            EXPECT_EQ(game.board[k][l], game.currentPlayer);
        }
    }
}


