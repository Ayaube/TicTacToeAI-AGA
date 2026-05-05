#ifndef PLATEAU_H
#define PLATEAU_H

#include <vector>
#include <chrono>
#include "main.h"

class Plateau {
public:
    Plateau();

    // --- Interface publique ---
    void jouerIA(int row, int col);      // enregistre le coup adverse (-1)
    void jouerNous(int row, int col);    // enregistre notre coup (+1)
    void choisirCoup(GameMove& out, const GameMove& dernierCoupAdverse);

    void afficher() const;

private:
    // Grille 9x9 : 0=vide, 1=nous, -1=IA adverse
    int m_grille[9][9];
    // Etat des 9 sous-plateaux : 0=en cours, 1=on a gagne, -1=adverse gagne, 2=nul
    int m_etat[3][3];

    // --- Utilitaires ---
    bool estJouable(int si, int sj) const;   // sous-plateau encore ouvert
    bool estPlein(int si, int sj) const;
    int  gagnantSous(int si, int sj) const;  // 1, -1, ou 0
    int  gagnantMeta() const;                // 1, -1, ou 0
    void mettreAJourEtat(int si, int sj);

    // Retourne les coups légaux étant donné le dernier coup joué
    std::vector<GameMove> coupsLegaux(const GameMove& dernierCoup) const;

    // --- Simulation (pour minimax) ---
    int  simulerJouer(int row, int col, int joueur); // retourne ancien etat sous-plateau
    void simulerAnnuler(int row, int col, int ancienEtat);

    // --- Minimax ---
    int minimax(GameMove dernierCoup, int profondeur,
                int alpha, int beta, int joueur,
                std::chrono::steady_clock::time_point deadline);

    // --- Evaluation ---
    int evaluer() const;
    int scoreLigneMeta(int a, int b, int c, int joueur) const;
    int scoreLigneSous(int a, int b, int c, int joueur) const;
    int evaluerSous(int si, int sj) const;
};

#endif
