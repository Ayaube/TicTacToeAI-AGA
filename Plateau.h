#ifndef PLATEAU_H
#define PLATEAU_H

#include <array>
#include <vector>
#include "main.h"

static const int SCORE_VICTOIRE =  1000000;
static const int SCORE_DEFAITE  = -1000000;
static const int PROFONDEUR_MAX = 9;
static const int MAX_MOVES      = 81; // nombre maximum de coups possibles

class Plateau {

private:
    std::array<std::array<int,9>,9> m_g; // grille 9x9
    std::array<std::array<int,3>,3> m_e; // etat des sous-plateaux

    int  jouerCoup(int row, int col, int joueur);
    void annulerCoup(int row, int col, int ancienEtat);
    int  etatSousPlateau(int si, int sj);

    // Remplit le tableau avec les coups legaux.
    int getCoupsLegauxFast(const GameMove& last, GameMove buf[MAX_MOVES]);

    int minimax(GameMove last, int depth, int alpha, int beta, int joueur);

    int evaluer();
    int urgenceMetaGrille(int joueur);

public:
    Plateau();
    ~Plateau();

    int  getCase(int row, int col);
    void setCase(int row, int col, int val);
    std::array<std::array<int,3>,3> getEtat() const { return m_e; }

    int  gagnant(int dL, int dC);
    void verifPlateau();
    bool estCondamne(int row, int col);
    bool estPlein(int si, int sj);
    int  gagnantMetaGrille();

    // Fonctions appelees par main.cpp.
    std::vector<GameMove> getCoupsLegaux(const GameMove& last);
    void prochainMove(GameMove& myMove, GameMove& lastMove);
};

#endif
