#include <iostream>
#include "main.h"
#include "Plateau.h"

using namespace std;

namespace {

constexpr unsigned int NB_GAMES = 100;

#if defined(NIVEAU_MEDIUM_2)
constexpr Level LEVEL = Level::MEDIUM_2;
#elif defined(NIVEAU_HARD_1)
constexpr Level LEVEL = Level::HARD_1;
#else
constexpr Level LEVEL = Level::MEDIUM_1;
#endif

constexpr Mode MODE = Mode::ARENA;
constexpr bool ALWAYS_PLAY_FIRST = false;
constexpr const char* ALIAS = "AGA";

bool isValidMove(const GameMove& move) {
    return move.row >= 0 && move.row < 9 && move.col >= 0 && move.col < 9;
}

} // namespace

int main()
{
    game.initialize(NB_GAMES, LEVEL, MODE, ALWAYS_PLAY_FIRST, ALIAS);

    while (!game.isAllGameFinish())
    {
        Plateau plateau;
        GameMove lastMove{-1, -1};

        while (!game.isFinish())
        {
            GameMove iaMove{-1, -1};
            game.getMove(iaMove);
            std::cerr << "IA move " << iaMove.row << " " << iaMove.col << std::endl;

            if (isValidMove(iaMove)) {
                plateau.setCase(iaMove.row, iaMove.col, -1);
                plateau.verifPlateau();
                lastMove = iaMove;
            }

            GameMove myMove{-1, -1};
            plateau.prochainMove(myMove, lastMove);
            if (!isValidMove(myMove)) {
                std::cerr << "No legal move found" << std::endl;
                break;
            }

            std::cerr << "Send move " << myMove.row << " " << myMove.col << std::endl;
            game.setMove(myMove);
            plateau.setCase(myMove.row, myMove.col, 1);
            plateau.verifPlateau();
            lastMove = myMove;
        }
    }

    return 0;
}
