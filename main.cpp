#include <iostream>
#include <vector>
#include "main.h"
using namespace std;

void isFull(vector<vector<int>> &plateau, vector<int> &vfull){
    for (vector<int> grille:plateau){
            if((grille[0] == 1) && (grille[4] == 1) && (grille[8] == 1)){
                cout << "Le joueur a gagné" << endl;
            }
            else if((grille[0] == -1) && (grille[4] == -1) && (grille[8] == -1)){
                cout << "L'IA a gagné" << endl;
            }

            else if((grille[6] == 1) && (grille[4] == 1) && (grille[2] == 1)){
                cout << "Le joueur a gagné" << endl;
            }
            else if((grille[6] == -1) && (grille[4] == -1) && (grille[2] == -1)){
                cout << "L'IA a gagné" << endl;
            }


            else if((grille[0] == 1) && (grille[3] == 1) && (grille[6] == 1)){
                cout << "Le joueur a gagné" << endl;
            }
            else if((grille[0] == -1) && (grille[3] == -1) && (grille[6] == -1)){
                cout << "L'IA a gagné" << endl;
            }

            else if((grille[1] == 1) && (grille[4] == 1) && (grille[7] == 1)){
                cout << "Le joueur a gagné" << endl;
            }
            else if((grille[1] == -1) && (grille[4] == -1) && (grille[7] == -1)){
                cout << "L'IA a gagné" << endl;
            }

            else if((grille[2] == 1) && (grille[5] == 1) && (grille[8] == 1)){
                cout << "Le joueur a gagné" << endl;
            }
            else if((grille[2] == -1) && (grille[5] == -1) && (grille[8] == -1)){
                cout << "L'IA a gagné" << endl;
            }


            else if((grille[0] == 1) && (grille[1] == 1) && (grille[2] == 1)){
                cout << "Le joueur a gagné" << endl;
            }
            else if((grille[0] == -1) && (grille[1] == -1) && (grille[2] == -1)){
                cout << "L'IA a gagné" << endl;
            }

            else if((grille[3] == 1) && (grille[4] == 1) && (grille[5] == 1)){
                cout << "Le joueur a gagné" << endl;
            }
            else if((grille[3] == -1) && (grille[4] == -1) && (grille[5] == -1)){
                cout << "L'IA a gagné" << endl;
            }

            else if((grille[6] == 1) && (grille[7] == 1) && (grille[8] == 1)){
                cout << "Le joueur a gagné" << endl;
            }
            else if((grille[6] == -1) && (grille[7] == -1) && (grille[8] == -1)){
                cout << "L'IA a gagné" << endl;
            }


    }
}

int main()
{
    // Game initialization
    game.initialize(1, Level::EASY_1, Mode::DEBUG, false, "Pseudo");

    int size = 9;
    vector<vector<int>> plateau(size, vector<int>(size, 0));
    vector<int> vfull(3,0);

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

            isFull(plateau,vfull);

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
            isFull(plateau,vfull);
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
