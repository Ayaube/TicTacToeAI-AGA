#include <iostream>
#include <vector>
#include "main.h"
#include "Plateau.h"

using namespace std;

int main()
{
    // Game initialization
    game.initialize(10, Level::MEDIUM_1, Mode::DEBUG, false, "Pseudo");

    int size = 9;
    vector<vector<int>> plateau(size, vector<int>(size, 0));
    vector<int> vfull(3,0);

    while (!game.isAllGameFinish())
    {
        Plateau plateau;
        GameMove myMove{1,1};

        while (!game.isFinish())
        {
            // Get IA move

            GameMove gameMove;
            game.getMove(gameMove);

            std::cerr << "IA move " << gameMove.row << " " << gameMove.col << std::endl;
            plateau.setCase(gameMove.row, gameMove.col, -1);
            plateau.verifPlateau();


            plateau.prochainMove(myMove, gameMove);

            // Send your move
            std::cerr << "Send move " << myMove.row << " " << myMove.col << std::endl;
            game.setMove(myMove);
            plateau.setCase(myMove.row, myMove.col, 1);
            plateau.verifPlateau();
        }
        plateau.affiche_plateau();

    }

    return 0;
}
