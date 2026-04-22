#include <iostream>
#include <vector>
#include "main.h"
using namespace std;


int main()
{
    // Game initialization
    game.initialize(1, Level::EASY_1, Mode::DEBUG, false, "Pseudo");

    vector<int> ligne;
    vector<vector<int>> grille;

    for (int i=0; i<3; i++){
        ligne.push_back(0);
    }

    for (int i=0; i<3; i++){
        grille.push_back(ligne);
    }

    while (!game.isAllGameFinish())
    {
        GameMove myMove{1,1};

        while (!game.isFinish())
        {
            // Get IA move
            GameMove gameMove;
            game.getMove(gameMove);
            std::cerr << "IA move " << gameMove.row << " " << gameMove.col << std::endl;
            grille[gameMove.row][gameMove.col] = -1;

            // Send your move
            std::cerr << "Send move " << myMove.row << " " << myMove.col << std::endl;
            game.setMove(myMove);
            grille[gameMove.row][gameMove.col] = 1;
        }
        for (vector<int> i:grille){
            for (int j:ligne){
                cout << j << " ";
            }
            cout << endl;
        }
    }

    return 0;
}
