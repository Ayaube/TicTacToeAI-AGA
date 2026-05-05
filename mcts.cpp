#include "MCTS.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>

using namespace std;
using namespace std::chrono;

// ============================================================
//  Etat
// ============================================================

Etat::Etat() : dernierCoup{-1, -1} {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            grille[i][j] = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            etat[i][j] = 0;
}

bool Etat::estPlein(int si, int sj) const {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grille[si*3+i][sj*3+j] == 0) return false;
    return true;
}

bool Etat::estJouable(int si, int sj) const {
    return etat[si][sj] == 0 && !estPlein(si, sj);
}

int Etat::gagnantSous(int si, int sj) const {
    int dL = si*3, dC = sj*3;
    for (int p : {1, -1}) {
        if (grille[dL][dC]==p   && grille[dL+1][dC+1]==p && grille[dL+2][dC+2]==p) return p;
        if (grille[dL][dC+2]==p && grille[dL+1][dC+1]==p && grille[dL+2][dC]==p)   return p;
        for (int i = 0; i < 3; i++)
            if (grille[dL+i][dC]==p && grille[dL+i][dC+1]==p && grille[dL+i][dC+2]==p) return p;
        for (int k = 0; k < 3; k++)
            if (grille[dL][dC+k]==p && grille[dL+1][dC+k]==p && grille[dL+2][dC+k]==p) return p;
    }
    return 0;
}

int Etat::gagnantMeta() const {
    for (int p : {1, -1}) {
        if (etat[0][0]==p && etat[1][1]==p && etat[2][2]==p) return p;
        if (etat[0][2]==p && etat[1][1]==p && etat[2][0]==p) return p;
        for (int i = 0; i < 3; i++)
            if (etat[i][0]==p && etat[i][1]==p && etat[i][2]==p) return p;
        for (int k = 0; k < 3; k++)
            if (etat[0][k]==p && etat[1][k]==p && etat[2][k]==p) return p;
    }
    return 0;
}

void Etat::jouer(int row, int col, int joueur) {
    grille[row][col] = joueur;
    dernierCoup = {row, col};
    int si = row/3, sj = col/3;
    if (etat[si][sj] == 0) {
        int g = gagnantSous(si, sj);
        if      (g != 0)          etat[si][sj] = g;
        else if (estPlein(si,sj)) etat[si][sj] = 2;
    }
}

vector<GameMove> Etat::coupsLegaux() const {
    vector<GameMove> coups;
    bool libre = true;

    if (dernierCoup.row >= 0 && dernierCoup.row < 9 &&
        dernierCoup.col >= 0 && dernierCoup.col < 9) {
        int ci = dernierCoup.row % 3;
        int cj = dernierCoup.col % 3;
        if (estJouable(ci, cj)) {
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if (grille[ci*3+i][cj*3+j] == 0)
                        coups.push_back({ci*3+i, cj*3+j});
            libre = false;
        }
    }

    if (libre) {
        for (int si = 0; si < 3; si++)
            for (int sj = 0; sj < 3; sj++)
                if (estJouable(si, sj))
                    for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++)
                            if (grille[si*3+i][sj*3+j] == 0)
                                coups.push_back({si*3+i, sj*3+j});
    }

    return coups;
}

bool Etat::estTermine() const {
    if (gagnantMeta() != 0) return true;
    for (int si = 0; si < 3; si++)
        for (int sj = 0; sj < 3; sj++)
            if (estJouable(si, sj)) return false;
    return true;
}

// ============================================================
//  Noeud
//
//  Convention : victoires et visites sont du point de vue
//  du joueur qui a JOUE ce coup (joueurQuiJoue du parent).
//  Autrement dit : si ce noeud a ete cree par le joueur 1,
//  victoires compte les victoires du joueur 1.
//  UCB1 est calcule depuis le parent : le parent maximise
//  le score de ses enfants du point de vue du joueur courant.
// ============================================================

Noeud::Noeud(const Etat& e, int joueurQuiVientDeJouer, GameMove coup, Noeud* par)
    : etat(e),
      joueurQuiAJoue(joueurQuiVientDeJouer),
      coupJoue(coup),
      parent(par),
      victoires(0.0),
      visites(0)
{
    coupsNonExplores = etat.coupsLegaux();
}

// UCB1 : appele par le PARENT pour choisir quel enfant explorer
// Le parent est le joueur -joueurQuiAJoue, donc il veut minimiser
// le score de ce noeud (du point de vue de joueurQuiAJoue).
// On retourne donc le taux de DEFAITE de joueurQuiAJoue,
// ce qui est equivalent au taux de victoire du parent.
double Noeud::ucb1(double C) const {
    if (visites == 0) return numeric_limits<double>::infinity();
    // taux de victoire du joueur PARENT = 1 - taux victoire de ce noeud
    double exploitation = 1.0 - (victoires / visites);
    double exploration  = C * sqrt(log(parent->visites) / visites);
    return exploitation + exploration;
}

// ============================================================
//  MCTS
// ============================================================

MCTS::MCTS(int seed) : m_rng(seed) {}

// Selection : descend en choisissant l'enfant avec meilleur UCB1
Noeud* MCTS::selectionner(Noeud* racine) {
    Noeud* n = racine;
    while (!n->etat.estTermine()) {
        if (!n->estPleinementExpanse()) return n;
        double best = -1.0;
        Noeud* bestEnfant = nullptr;
        for (auto& e : n->enfants) {
            double s = e->ucb1();
            if (s > best) { best = s; bestEnfant = e.get(); }
        }
        if (!bestEnfant) return n;
        n = bestEnfant;
    }
    return n;
}

// Expansion : ajoute un enfant pour un coup non encore explore
Noeud* MCTS::expanser(Noeud* noeud) {
    if (noeud->etat.estTermine()) return noeud;

    // Choisir un coup non explore au hasard
    int idx = uniform_int_distribution<int>(0, (int)noeud->coupsNonExplores.size()-1)(m_rng);
    GameMove coup = noeud->coupsNonExplores[idx];
    noeud->coupsNonExplores.erase(noeud->coupsNonExplores.begin() + idx);

    // Le joueur qui joue dans ce noeud est l'opposé du joueur qui a joue pour arriver ici
    int joueurSuivant = -noeud->joueurQuiAJoue;

    Etat nouvelEtat = noeud->etat;
    nouvelEtat.jouer(coup.row, coup.col, joueurSuivant);

    auto enfant = make_unique<Noeud>(nouvelEtat, joueurSuivant, coup, noeud);
    Noeud* ptr = enfant.get();
    noeud->enfants.push_back(move(enfant));
    return ptr;
}

// Simulation : playout aleatoire jusqu'a la fin
// Retourne le gagnant (1, -1, ou 0)
int MCTS::simuler(const Etat& etatDepart, int joueurSuivant) {
    Etat e = etatDepart;
    int joueur = joueurSuivant;
    int nbCoups = 0;

    while (!e.estTermine() && nbCoups < 150) {
        vector<GameMove> coups = e.coupsLegaux();
        if (coups.empty()) break;

        GameMove choix;

        // Heuristique legere : chercher un coup gagnant immediat
        bool trouve = false;
        for (const GameMove& c : coups) {
            Etat test = e;
            test.jouer(c.row, c.col, joueur);
            if (test.gagnantMeta() == joueur) {
                choix = c;
                trouve = true;
                break;
            }
        }

        if (!trouve) {
            // Sinon chercher a bloquer une victoire immediate adverse
            for (const GameMove& c : coups) {
                Etat test = e;
                test.jouer(c.row, c.col, -joueur);
                if (test.gagnantMeta() == -joueur) {
                    choix = c;
                    trouve = true;
                    break;
                }
            }
        }

        if (!trouve) {
            // Sinon coup aleatoire
            choix = coups[uniform_int_distribution<int>(0, (int)coups.size()-1)(m_rng)];
        }

        e.jouer(choix.row, choix.col, joueur);
        joueur = -joueur;
        nbCoups++;
    }

    return e.gagnantMeta();
}

// Retropropagation : remonte le resultat
// resultat = 1 si joueur 1 a gagne, -1 si joueur -1 a gagne, 0 si egalite
void MCTS::retropropager(Noeud* noeud, int resultat) {
    while (noeud != nullptr) {
        noeud->visites++;
        // Ce noeud a ete cree par joueurQuiAJoue
        // On incremente ses victoires si ce joueur a gagne
        if (resultat == noeud->joueurQuiAJoue)
            noeud->victoires += 1.0;
        else if (resultat == 0)
            noeud->victoires += 0.5;
        noeud = noeud->parent;
    }
}

// Point d'entree
GameMove MCTS::choisirCoup(const Etat& etatInitial, int joueurQuiJoue, int temps_ms) {

    // La racine represente l'etat AVANT notre coup.
    // joueurQuiAJoue pour la racine = le joueur adverse (celui qui vient de jouer)
    // Le premier enfant sera cree avec joueurQuiJoue = nous
    auto racine = make_unique<Noeud>(etatInitial, -joueurQuiJoue, GameMove{-1,-1}, nullptr);

    auto deadline = steady_clock::now() + milliseconds(temps_ms);
    int  iterations = 0;

    while (steady_clock::now() < deadline) {
        Noeud* noeud = selectionner(racine.get());
        if (!noeud->etat.estTermine())
            noeud = expanser(noeud);
        int resultat = simuler(noeud->etat, -noeud->joueurQuiAJoue);
        retropropager(noeud, resultat);
        iterations++;
    }

    cerr << "[MCTS] " << iterations << " iterations\n";

    // Choisir l'enfant le plus visite parmi les enfants de la racine
    // (ces enfants correspondent a nos coups possibles)
    int bestV = -1;
    GameMove bestCoup{-1, -1};

    for (const auto& enfant : racine->enfants) {
        if (enfant->visites > bestV) {
            bestV    = enfant->visites;
            bestCoup = enfant->coupJoue;
        }
    }

    cerr << "[MCTS] meilleur coup=(" << bestCoup.row << ","
         << bestCoup.col << ") visites=" << bestV << "\n";

    // Securite si aucun enfant
    if (bestCoup.row == -1) {
        auto coups = etatInitial.coupsLegaux();
        if (!coups.empty()) bestCoup = coups[0];
    }

    return bestCoup;
}
