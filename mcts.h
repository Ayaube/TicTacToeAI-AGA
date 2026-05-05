#ifndef MCTS_H
#define MCTS_H

#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include "main.h"

// ============================================================
//  Etat du jeu (autonome, pas besoin de Plateau)
// ============================================================

struct Etat {
    int grille[9][9];  // 0=vide, 1=nous, -1=adverse
    int etat[3][3];    // 0=ouvert, 1=nous, -1=adverse, 2=nul
    GameMove dernierCoup;

    Etat();
    Etat(const Etat&) = default;

    bool estPlein(int si, int sj) const;
    bool estJouable(int si, int sj) const;
    int  gagnantSous(int si, int sj) const;
    int  gagnantMeta() const;
    void jouer(int row, int col, int joueur);
    std::vector<GameMove> coupsLegaux() const;
    bool estTermine() const;
};

// ============================================================
//  Noeud MCTS
// ============================================================

struct Noeud {
    Etat   etat;
    int    joueurQuiAJoue;   // joueur qui a joue pour arriver a ce noeud
    GameMove coupJoue;
    Noeud* parent;

    std::vector<std::unique_ptr<Noeud>> enfants;
    std::vector<GameMove> coupsNonExplores;

    double victoires;
    int    visites;

    Noeud(const Etat& e, int joueurQuiAJoue, GameMove coup, Noeud* par);

    // UCB1 du point de vue du joueur parent
    double ucb1(double C = 1.41421356) const;

    bool estPleinementExpanse() const { return coupsNonExplores.empty(); }
};

// ============================================================
//  Moteur MCTS
// ============================================================

class MCTS {
public:
    explicit MCTS(int seed = 42);

    GameMove choisirCoup(const Etat& etatInitial,
                         int joueurQuiJoue,
                         int temps_ms = 300);

private:
    std::mt19937 m_rng;

    Noeud* selectionner(Noeud* racine);
    Noeud* expanser(Noeud* noeud);
    int    simuler(const Etat& etat, int joueurSuivant);
    void   retropropager(Noeud* noeud, int resultat);
};

#endif
