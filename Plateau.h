#ifndef PLATEAU
#define PLATEAU
#include <iostream>
#include <vector>
#include "main.h"

class Plateau{
    private:
        std::vector<std::vector<int>> m_grille;
        std::vector<std::vector<int>> m_etat;

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
        int evaluerPlateau();
        int evaluerSousPlateau(int startR, int startC);
        std::vector<std::pair<int,int>> getCoupsValides(int cibleL, int cibleC);
        int prioriteCoup(int r, int c, int joueur);
        int etatSousPlateau(int br, int bc);

};


#endif // PLATEAU
