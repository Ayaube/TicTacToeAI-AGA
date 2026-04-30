#ifndef PLATEAU
#define PLATEAU
#include <iostream>
#include <vector>
#include <cstdint>
#include "main.h"

class Plateau{
    private:
        std::vector<std::vector<int>> m_grille;
        std::vector<std::vector<int>> m_etat;

        struct TTEntry {
            uint64_t key;
            int      score;
            int16_t  depth;
            uint8_t  flag; // 0=exact, 1=lowerbound, 2=upperbound
            uint8_t  gen;
            int8_t   best_r, best_c;
        };
        static const int TT_BITS = 18;          // 256 K entrées
        static const int TT_SIZE = 1 << TT_BITS;
        TTEntry* m_tt;
        uint8_t  m_gen;

        int m_killers[12][2][2]; // [depth][slot][r/c]
        int m_history[9][9];
        uint64_t m_zobrist[9][9][3];
        uint64_t m_zobristCible[10];
        uint64_t m_hash;

        void initZobrist();
        uint64_t computeHash(int cibleL, int cibleC) const;

    public:
        Plateau();
        ~Plateau();

        int getCase(int row, int col);
        void setCase(int row, int col, int val);

        void affiche_plateau();
        int gagnant(int dL, int dC);
        void verifPlateau();
        void prochainMove(GameMove& myMove, GameMove& lastMove);
        bool estCondamne(int row, int col);
        int minimax(int profondeur, int alpha, int beta, bool estMax, int cibleL, int cibleC);
        int quiesce(int alpha, int beta, bool estMax, int cibleL, int cibleC, int maxPlies);
        int evaluerPlateau();
        int evaluerSousPlateau(int startR, int startC);
        std::vector<std::pair<int,int>> getCoupsValides(int cibleL, int cibleC);
        int prioriteCoup(int r, int c, int joueur);
        int etatSousPlateau(int br, int bc);
};

#endif // PLATEAU
