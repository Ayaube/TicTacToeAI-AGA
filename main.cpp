#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include "main.h"
#include "Plateau.h"

using namespace std;

namespace {

struct RunConfig {
    Mode mode = Mode::ARENA;
    Level level = Level::MEDIUM_1;
    unsigned int nbGames = 100;
    bool alwaysPlayFirst = false;
    string alias = "AGA";
};

bool isValidMove(const GameMove& move) {
    return move.row >= 0 && move.row < 9 && move.col >= 0 && move.col < 9;
}

Mode parseMode(const string& value) {
    if (value == "debug" || value == "DEBUG") {
        return Mode::DEBUG;
    }
    return Mode::ARENA;
}

Level parseLevel(const string& value) {
    if (value == "easy1") return Level::EASY_1;
    if (value == "easy2") return Level::EASY_2;
    if (value == "med1") return Level::MEDIUM_1;
    if (value == "med2") return Level::MEDIUM_2;
    if (value == "hard1") return Level::HARD_1;
    if (value == "hard2") return Level::HARD_2;
    if (value == "vhard1") return Level::VERY_HARD_1;
    if (value == "vhard2") return Level::VERY_HARD_2;
    return Level::MEDIUM_1;
}

RunConfig readConfig(int argc, char** argv) {
    RunConfig cfg;
    if (argc > 1) cfg.mode = parseMode(argv[1]);
    if (argc > 2) cfg.level = parseLevel(argv[2]);
    if (argc > 3) cfg.nbGames = static_cast<unsigned int>(std::max(1, atoi(argv[3])));
    if (argc > 4) cfg.alwaysPlayFirst = (atoi(argv[4]) != 0);
    if (argc > 5 && argv[5] && argv[5][0] != '\0') cfg.alias = argv[5];
    return cfg;
}

} // namespace

int main(int argc, char** argv)
{
    RunConfig cfg = readConfig(argc, argv);
    game.initialize(cfg.nbGames, cfg.level, cfg.mode, cfg.alwaysPlayFirst, cfg.alias);

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
