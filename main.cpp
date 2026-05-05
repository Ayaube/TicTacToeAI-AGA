#include <iostream>
#include "main.h"
#include "Plateau.h"

using namespace std;

int main()
{
    game.initialize(10, Level::MEDIUM_2, Mode::ARENA, false, "Pseudo");

    while (!game.isAllGameFinish())
    {
        Plateau plateau;
        GameMove dernierCoup{-1, -1};

        while (!game.isFinish())
        {
            // --- Coup adverse ---
            GameMove coupAdverse{-1, -1};
            game.getMove(coupAdverse);

            bool valide = (coupAdverse.row >= 0 && coupAdverse.row < 9 &&
                           coupAdverse.col >= 0 && coupAdverse.col < 9);

            if (valide) {
                cerr << "[Adverse] (" << coupAdverse.row << "," << coupAdverse.col << ")\n";
                plateau.jouerIA(coupAdverse.row, coupAdverse.col);
                dernierCoup = coupAdverse;
            } else {
                cerr << "[Adverse] pas de coup -> on joue en premier\n";
                dernierCoup = {-1, -1};
            }

            // --- Notre coup via Minimax alpha-beta ---
            GameMove monCoup{-1, -1};
            plateau.choisirCoup(monCoup, dernierCoup);

            cerr << "[Nous]    (" << monCoup.row << "," << monCoup.col << ")\n";

            game.setMove(monCoup);
            plateau.jouerNous(monCoup.row, monCoup.col);
            dernierCoup = monCoup;
        }

        Winner w = game.getWinner();
        if      (w == Winner::PLAYER)        cerr << ">>> VICTOIRE <<<\n";
        else if (w == Winner::IA)            cerr << ">>> DEFAITE <<<\n";
        else if (w == Winner::IA_AND_PLAYER) cerr << ">>> EGALITE <<<\n";
    }
    return 0;
}
