#include <iostream>
#include <vector>
#include "main.h"
#include "Plateau.h"

using namespace std;

int main()
{
    // Initialisation de la partie
    game.initialize(10, Level::MEDIUM_2, Mode::ARENA, false, "AGA");

    while (!game.isAllGameFinish())
    {
        Plateau plateau;

        // lastMove stocke le dernier coup joue (par l'IA adverse ou par nous)
        // On l'initialise a {-1,-1} pour signaler "pas encore de coup joue"
        GameMove lastMove{-1, -1};
        GameMove myMove{-1, -1};

        while (!game.isFinish())
        {
            // Recuperer le coup de l'IA adverse
            GameMove gameMove{-1, -1};
            game.getMove(gameMove);

            std::cerr << "IA move " << gameMove.row << " " << gameMove.col << std::endl;

            // Si l'IA a joue un coup valide, on le place sur le plateau
            if (gameMove.row >= 0 && gameMove.row < 9 &&
                gameMove.col >= 0 && gameMove.col < 9)
            {
                plateau.setCase(gameMove.row, gameMove.col, -1);
                plateau.verifPlateau();
                lastMove = gameMove; // le dernier coup joue est celui de l'IA adverse
            }

            // Calculer notre meilleur coup via minimax
            plateau.prochainMove(myMove, lastMove);

            // Envoyer notre coup
            std::cerr << "Send move " << myMove.row << " " << myMove.col << std::endl;
            game.setMove(myMove);
            plateau.setCase(myMove.row, myMove.col, 1);
            plateau.verifPlateau();

            // Notre coup devient le dernier coup joue
            lastMove = myMove;
        }

        plateau.affiche_plateau();
    }

    return 0;
}
