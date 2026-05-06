#include "main.h"
#include "Plateau.h"

int main()
{
    game.initialize(10, Level::MEDIUM_1, Mode::ARENA, false, "AGA");

    while (!game.isAllGameFinish())
    {
        Plateau plateau;
        GameMove myMove{1,1};

        while (!game.isFinish())
        {
            GameMove gameMove;
            game.getMove(gameMove);

            if (gameMove.row != -1) {
                plateau.setCase(gameMove.row, gameMove.col, -1);
                plateau.verifPlateau();
            }

            plateau.prochainMove(myMove, gameMove);

            game.setMove(myMove);
            plateau.setCase(myMove.row, myMove.col, 1);
            plateau.verifPlateau();
        }
    }

    return 0;
}
