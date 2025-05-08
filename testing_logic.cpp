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
FRIEND_TEST(func_logic,checkwin);
FRIEND_TEST(func_logic,checkdraw);
FRIEND_TEST(func_logic,checkswap);
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
    }//can't be tested because it is a terminal function

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
    }//can test if it takes into consideration valid inputs DONE

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
    }//will force values on the board through the makemove function DONE

    bool checkDraw() {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (board[i][j] == ' ')
                    return false;
        return true;
    }//same test case as the function above

    void switchPlayer() {
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }//test if it switches correctly DONE

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
TEST(func_logic,checkswap){
TicTacToe checkswaptest;
//testing the change from X to O since the default value is O
EXPECT_EQ(checkswaptest.currentPlayer,'X');//default value must be X
checkswaptest.switchPlayer();
EXPECT_EQ(checkswaptest.currentPlayer,'O');//must be switched to O
checkswaptest.switchPlayer();
EXPECT_EQ(checkswaptest.currentPlayer,'X');//switched to X again
}

//testing the winning condition function
TEST(func_logic,checkwin){
TicTacToe checkwinTest;
//setting a specific row
for(int k=0;k<3;k++)
checkwinTest.board[0][k] = checkwinTest.currentPlayer;
//checking for the win on row 1
EXPECT_EQ(checkwinTest.checkWin(),true);
for(int k=0;k<3;k++){
checkwinTest.board[0][k] = ' ';//resetting the board
checkwinTest.board[1][k] = checkwinTest.currentPlayer;
}
//checks for the win on row 2
EXPECT_EQ(checkwinTest.checkWin(),true);
for(int k=0;k<3;k++){
checkwinTest.board[1][k] = ' ';//resetting the board
checkwinTest.board[2][k] = checkwinTest.currentPlayer;
}
//checks for the win on row 3
EXPECT_EQ(checkwinTest.checkWin(),true);

for(int k=0;k<3;k++){
checkwinTest.board[2][k] = ' ';//resetting the board
checkwinTest.board[k][0] = checkwinTest.currentPlayer;}
//checks for the win on column 1
EXPECT_EQ(checkwinTest.checkWin(),true);

for(int k=0;k<3;k++){
checkwinTest.board[k][0] = ' ';//resetting the board
checkwinTest.board[k][1] = checkwinTest.currentPlayer;}
//checks for the win on column 2
EXPECT_EQ(checkwinTest.checkWin(),true);

for(int k=0;k<3;k++){
checkwinTest.board[k][1] = ' ';//resetting the board
checkwinTest.board[k][2] = checkwinTest.currentPlayer;
}
//checks for the win on column 3
EXPECT_EQ(checkwinTest.checkWin(),true);

for(int k=0;k<3;k++){
checkwinTest.board[k][2] = ' ';//resetting the board
checkwinTest.board[k][k] = checkwinTest.currentPlayer;}
//checks for the win on the diagonal
EXPECT_EQ(checkwinTest.checkWin(),true);

for(int k=0;k<3;k++){
checkwinTest.board[k][k] = ' ';//resetting the board
checkwinTest.board[2-k][k] = checkwinTest.currentPlayer;}
//checks for the win on the other diagonal
EXPECT_EQ(checkwinTest.checkWin(),true);
for(int k=0;k<3;k++)
checkwinTest.board[2-k][k] = ' ';//resetting the board
//all win conditions have been tested now to check if it doesnt detect a win
EXPECT_EQ(checkwinTest.checkWin(),false);

}
//testing the checkdraw function
TEST(func_logic,checkdraw){
TicTacToe checkdrawtest;
/*since not all draw cases can be written easily we will prove a general case
will put a winning case for row,column,draw then will check if it
isnt a draw*/
for(int k=0;k<3;k++)
checkdrawtest.board[0][k] = checkdrawtest.currentPlayer;
EXPECT_EQ(checkdrawtest.checkDraw(),false);
for(int k=0;k<3;k++)
{
checkdrawtest.board[0][k] = ' ';
checkdrawtest.board[k][0] = checkdrawtest.currentPlayer;
}
EXPECT_EQ(checkdrawtest.checkDraw(),false);
for(int k=0;k<3;k++)
{
checkdrawtest.board[k][0]=' ';
checkdrawtest.board[k][k]=checkdrawtest.currentPlayer;
}
EXPECT_EQ(checkdrawtest.checkDraw(),false);
for(int k=0;k<3;k++)
{
checkdrawtest.board[k][k]=' ';
}
EXPECT_EQ(checkdrawtest.checkDraw(),false);
//putting in a draw case
checkdrawtest.board[1][1]='X';
checkdrawtest.board[0][2]='O';
checkdrawtest.board[0][1]='X';
checkdrawtest.board[2][1]='O';
checkdrawtest.board[1][2]='X';
checkdrawtest.board[1][0]='O';
checkdrawtest.board[0][0]='X';
checkdrawtest.board[2][2]='O';
checkdrawtest.board[2][0]='X';
EXPECT_EQ(checkdrawtest.checkDraw(),true);
}

//testing the makemove function
TEST(func_logic,makemove){
TicTacToe MakemoveTest;
//inputting a wrong row below the valid values
EXPECT_EQ(MakemoveTest.makeMove(-1,0),false);
//inputting a wrong row above the valid values
EXPECT_EQ(MakemoveTest.makeMove(3,0),false);
//inputting a wrong column above the valid values
EXPECT_EQ(MakemoveTest.makeMove(0,3),false);
//inputting a wrong column below the valid values
EXPECT_EQ(MakemoveTest.makeMove(0,-1),false);
//inputting correct rows and columns:
for(int k=0;k<3;k++){
for(int l=0;l<3;l++){
EXPECT_EQ(MakemoveTest.makeMove(k,l),true);}}
}

int main(int argc, char** argv) {
 testing::InitGoogleTest(&argc,argv);
sigreturn; RUN_ALL_TESTS();
}
