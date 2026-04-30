#include <iostream>
#include <vector>
#include <algorithm>
#include "Plateau.h"
#include "main.h"

using namespace std;

Plateau::Plateau() : m_grille(9, vector<int>(9, 0)), m_etat(3, vector<int>(3, 0)) {
}

Plateau::~Plateau(){}

int Plateau::getCase(int row, int col) {
    return m_grille[row][col];
}

void Plateau::setCase(int row, int col, int val) {
    m_grille[row][col] = val;
}

void Plateau::affiche_plateau(){
    for (vector<int> grille:m_grille){
        for (int j:grille){
            cout << j;
        }
        cout << " " << endl;
    }
}

int Plateau::gagnant(int dL, int dC) {
    for (int joueur : {1, -1}) {
        // Diagonales
        if (getCase(dL, dC) == joueur && getCase(dL+1, dC+1) == joueur && getCase(dL+2, dC+2) == joueur)
            return joueur;
        if (getCase(dL, dC+2) == joueur && getCase(dL+1, dC+1) == joueur && getCase(dL+2, dC) == joueur)
            return joueur;
        // Lignes
        for (int i = 0; i < 3; i++)
            if (getCase(dL+i, dC) == joueur && getCase(dL+i, dC+1) == joueur && getCase(dL+i, dC+2) == joueur)
                return joueur;
        // Colonnes
        for (int j = 0; j < 3; j++)
            if (getCase(dL, dC+j) == joueur && getCase(dL+1, dC+j) == joueur && getCase(dL+2, dC+j) == joueur)
                return joueur;
    }
    return 0;
}

void Plateau::verifPlateau(){
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (m_etat[i][j] != 0)
                continue;
            int result = gagnant(i * 3, j * 3);
            if (result != 0) {
                m_etat[i][j] = result;
                cout << (result == 1 ? "Joueur" : "IA")
                     << " gagne sous-plateau (" << i << "," << j << ")" << endl;
            } else {
                // Sous-plateau plein sans vainqueur → nul, marquer 2
                bool plein = true;
                for (int di = 0; di < 3 && plein; di++)
                    for (int dj = 0; dj < 3 && plein; dj++)
                        if (getCase(i*3+di, j*3+dj) == 0)
                            plein = false;
                if (plein)
                    m_etat[i][j] = 2;
            }
        }
    }
}



int Plateau::prioriteCoup(int r, int c, int joueur) {
    int score = 0;
    int lr = r % 3, lc = c % 3;

    if (lr == 1 && lc == 1) score += 4;       // centre
    else if (lr != 1 && lc != 1) score += 2;  // coin

    // Envoie l'adversaire dans un plateau condamné → il joue librement → mauvais
    if (estCondamne(lr * 3, lc * 3)) score -= 6;

    return score;
}

// Liste des coups possibles dans un sous-plateau (ou partout si cibleL == -1)
vector<pair<int,int>> Plateau::getCoupsValides(int cibleL, int cibleC) {
    vector<pair<int,int>> coups;

    if (cibleL == -1) {
        // Coup libre : toutes les cases vides dans les sous-plateaux non condamnés
        for (int r = 0; r < 9; r++)
            for (int c = 0; c < 9; c++)
                if (getCase(r, c) == 0 && !estCondamne(r, c))
                    coups.push_back({r, c});
    } else {
        // Coups dans le sous-plateau ciblé
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (getCase(cibleL + i, cibleC + j) == 0)
                    coups.push_back({cibleL + i, cibleC + j});
    }
    return coups;
}

int Plateau::evaluerSousPlateau(int startR, int startC) {
    int score = 0;
    int lignes[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };

    for (auto& ligne : lignes) {
        int nous = 0, eux = 0;
        for (auto& pos : ligne) {
            int val = getCase(startR + pos[0], startC + pos[1]);
            if (val == 1) nous++;
            else if (val == -1) eux++;
        }
        // Ligne ouverte pour nous (aucun ennemi)
        if (eux == 0) {
            if (nous == 2) score += 10;
            else if (nous == 1) score += 1;
        }
        // Ligne ouverte pour l'IA (aucun allié)
        if (nous == 0) {
            if (eux == 2) score -= 10;
            else if (eux == 1) score -= 1;
        }
    }
    return score;
}

int Plateau::evaluerPlateau() {
    int score = 0;

    int metaLignes[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };

    // Évaluer les sous-plateaux individuels
    for (int bl = 0; bl < 3; bl++) {
        for (int bc = 0; bc < 3; bc++) {
            int poids = (bl == 1 && bc == 1) ? 3 :
                        (bl % 2 == 0 && bc % 2 == 0) ? 2 : 1;
            if (m_etat[bl][bc] == 1)       score += 100 * poids;
            else if (m_etat[bl][bc] == -1) score -= 100 * poids;
            else if (m_etat[bl][bc] == 0)  score += evaluerSousPlateau(bl * 3, bc * 3) * poids;
        }
    }

    // Évaluer les alignements méta — seules les cases ouvertes (m_etat==0) comptent
    for (auto& ligne : metaLignes) {
        int nous = 0, eux = 0, ouverts = 0;
        for (auto& pos : ligne) {
            int e = m_etat[pos[0]][pos[1]];
            if (e == 1)      nous++;
            else if (e == -1) eux++;
            else if (e == 0)  ouverts++;
            // e==2 (nul) bloque la ligne, ne compte pas
        }
        // Menace de victoire méta (2 gagnés + 1 sub-plateau encore prenable)
        if (eux == 0) {
            if      (nous == 2 && ouverts == 1) score += 250;
            else if (nous == 1 && ouverts == 2) score += 20;
        }
        if (nous == 0) {
            if      (eux == 2 && ouverts == 1) score -= 250;
            else if (eux == 1 && ouverts == 2) score -= 20;
        }
    }

    return score;
}

int Plateau::minimax(int profondeur, int alpha, int beta, bool estMax, int cibleL, int cibleC) {
    // Vérifier si quelqu'un a gagné le jeu global
    // (on réutilise gagnant sur le méta-plateau via m_etat)
    for (auto& ligne : vector<vector<pair<int,int>>>{
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    }) {
        int v0 = m_etat[ligne[0].first][ligne[0].second];
        int v1 = m_etat[ligne[1].first][ligne[1].second];
        int v2 = m_etat[ligne[2].first][ligne[2].second];
        if ((v0 == 1 || v0 == -1) && v0 == v1 && v1 == v2) {
            return v0 == 1 ? (1000 + profondeur) : -(1000 + profondeur);
        }
    }

    // Profondeur atteinte → évaluation heuristique
    if (profondeur == 0) return evaluerPlateau();

    // Récupérer les coups valides
    auto coups = getCoupsValides(cibleL, cibleC);
    if (coups.empty()) return evaluerPlateau();

    int joueur = estMax ? 1 : -1;

    sort(coups.begin(), coups.end(), [&](auto& a, auto& b) {
        return prioriteCoup(a.first, a.second, joueur) > prioriteCoup(b.first, b.second, joueur);
    });

    if (estMax) {
        int best = -9999;
        for (auto [r, c] : coups) {
            // Jouer le coup
            setCase(r, c, joueur);
            int oldEtat = m_etat[r/3][c/3];
            int newEtat = etatSousPlateau(r/3, c/3);
            if (newEtat != 0) m_etat[r/3][c/3] = newEtat;

            // Calculer la cible suivante
            int newCibleL = (r % 3) * 3;
            int newCibleC = (c % 3) * 3;
            if (estCondamne(newCibleL, newCibleC)) {
                newCibleL = -1;
                newCibleC = -1;
            }

            int score = minimax(profondeur - 1, alpha, beta, false, newCibleL, newCibleC);

            // Annuler le coup
            setCase(r, c, 0);
            m_etat[r/3][c/3] = oldEtat;

            best = max(best, score);
            alpha = max(alpha, best);
            if (beta <= alpha) break;
        }
        return best;
    } else {
        int best = 9999;
        for (auto [r, c] : coups) {
            setCase(r, c, joueur);
            int oldEtat = m_etat[r/3][c/3];
            int newEtat = etatSousPlateau(r/3, c/3);
            if (newEtat != 0) m_etat[r/3][c/3] = newEtat;

            int newCibleL = (r % 3) * 3;
            int newCibleC = (c % 3) * 3;
            if (estCondamne(newCibleL, newCibleC)) {
                newCibleL = -1;
                newCibleC = -1;
            }

            int score = minimax(profondeur - 1, alpha, beta, true, newCibleL, newCibleC);

            setCase(r, c, 0);
            m_etat[r/3][c/3] = oldEtat;

            best = min(best, score);
            beta = min(beta, best);
            if (beta <= alpha) break;
        }
        return best;
    }
}

void Plateau::prochainMove(GameMove &myMove, GameMove &lastMove) {
    int cibleL, cibleC;

    // Premier coup ou coup libre explicite
    if (lastMove.row == -1) {
        cibleL = -1;
        cibleC = -1;
    } else {
        cibleL = (lastMove.row % 3) * 3;
        cibleC = (lastMove.col % 3) * 3;
        // Si le sous-plateau cible est condamné → coup libre
        if (estCondamne(cibleL, cibleC)) {
            cibleL = -1;
            cibleC = -1;
        }
    }

    auto coups = getCoupsValides(cibleL, cibleC);

    // Fallback coup libre si le sous-plateau ciblé est plein
    if (coups.empty() && cibleL != -1) {
        cibleL = -1;
        cibleC = -1;
        coups = getCoupsValides(-1, -1);
    }

    if (coups.empty()) return;

    int meilleurScore = -99999;
    int profondeur = 5;

    for (auto [r, c] : coups) {
        // Simuler le coup
        setCase(r, c, 1);
        int oldEtat = m_etat[r/3][c/3];
        int newEtat = etatSousPlateau(r/3, c/3);
        if (newEtat != 0) m_etat[r/3][c/3] = newEtat;

        // Cible suivante pour l'IA
        int newCibleL = (r % 3) * 3;
        int newCibleC = (c % 3) * 3;
        if (estCondamne(newCibleL, newCibleC)) {
            newCibleL = -1;
            newCibleC = -1;
        }

        int score = minimax(profondeur - 1, -99999, 99999, false, newCibleL, newCibleC);

        // Annuler
        setCase(r, c, 0);
        m_etat[r/3][c/3] = oldEtat;

        if (score > meilleurScore) {
            meilleurScore = score;
            myMove.row = r;
            myMove.col = c;
        }
    }
}

bool Plateau::estCondamne(int row, int col) {
    int sousI = row / 3;
    int sousJ = col / 3;
    return m_etat[sousI][sousJ] != 0;
}

int Plateau::etatSousPlateau(int br, int bc) {
    int res = gagnant(br * 3, bc * 3);
    if (res != 0) return res;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (getCase(br*3+i, bc*3+j) == 0) return 0;
    return 2;
}
