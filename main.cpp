#include <iostream>
#include <vector>
#include "main.h"
using namespace std;

bool estPlein(vector<vector<int>>& plateau, int debutL, int debutC) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (plateau[debutL+i][debutC+j] == 0)
                cout << "plein" << endl;
                return false;
    return true;
}

int main()
{
    // Game initialization
    game.initialize(1, Level::EASY_1, Mode::DEBUG, false, "Pseudo");

    int size = 9;
    vector<vector<int>> plateau(size, vector<int>(size, 0));


    while (!game.isAllGameFinish())
    {
        GameMove myMove{1,1};

        while (!game.isFinish())
        {
            // Get IA move

            GameMove gameMove;
            game.getMove(gameMove);

            std::cerr << "IA move " << gameMove.row << " " << gameMove.col << std::endl;
            plateau[gameMove.row][gameMove.col] = -1;

            estPlein(plateau,gameMove.row,gameMove.col);

            if(plateau[1][1]==0){
                myMove.row=1;
                myMove.col=1;
            }
            else if(plateau[0][1] == 0){
                myMove.row=0;
                myMove.col=1;
            }
            else if(plateau[0][2]==0){
                myMove.row=0;
                myMove.col=2;
            }
            else if(plateau[2][1]==0){
                myMove.row=2;
                myMove.col=1;
            }
            else if(plateau[2][2]==0){
                myMove.row=2;
                myMove.col=2;
            }
            else if(plateau[0][0]==0){
                myMove.row=0;
                myMove.col=0;
            }
            else if(plateau[1][0]==0){
                myMove.row=1;
                myMove.col=0;
            }
            else if(plateau[1][1]==0){
                myMove.row=1;
                myMove.col=1;
            }
            else if(plateau[2][2]==0){
                myMove.row=2;
                myMove.col=2;
            }
            else if(plateau[1][2]==0){
                myMove.row=1;
                myMove.col=2;
            }

            // Send your move
            std::cerr << "Send move " << myMove.row << " " << myMove.col << std::endl;
            game.setMove(myMove);
            plateau[myMove.row][myMove.col] = 1;
            estPlein(plateau,myMove.row,myMove.col);
        }

        for (vector<int> grille:plateau){
            for (int j:grille){
                cout << j;
            }
            cout << " " << endl;
        }

    }

    return 0;
}
