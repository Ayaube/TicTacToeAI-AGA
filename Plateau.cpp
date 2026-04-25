#include <iostream>
#include <vector>
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
                continue;  // déjà condamné, on skip
            int result = gagnant(i * 3, j * 3);
            if (result != 0) {
                m_etat[i][j] = result;
                cout << (result == 1 ? "Joueur" : "IA")
                     << " gagne sous-plateau (" << i << "," << j << ")" << endl;
            }
        }
    }
}

void Plateau::prochainMove(GameMove &myMove, GameMove &lastMove){
    int cibleL = (lastMove.row % 3) * 3;
    int cibleC = (lastMove.col % 3) * 3;
    bool found = false;

    // Si le sous-plateau cible n'est pas condamné, jouer dedans
    if (!estCondamne(cibleL, cibleC)) {
        // 1. Peut-on gagner ce sous-plateau ? (aligner 3)
        for (int joueur : {1, -1}) {
            int ordre[][2] = {{0,0},{0,1},{0,2},{1,0},{1,1},{1,2},{2,0},{2,1},{2,2}};
            for (auto& pos : ordre) {
                int r = cibleL + pos[0];
                int c = cibleC + pos[1];
                if (getCase(r, c) == 0) {
                    // Simuler le coup
                    setCase(r, c, joueur == 1 ? 1 : -1);
                    int result = gagnant(cibleL, cibleC);
                    setCase(r, c, 0); // annuler

                    if (result == 1) {
                        // On peut gagner, on joue là
                        myMove.row = r;
                        myMove.col = c;
                        return;
                    }
                    if (result == -1 && joueur == -1) {
                        // L'IA pourrait gagner ici, on bloque
                        myMove.row = r;
                        myMove.col = c;
                        found = true;
                    }
                }
            }
        }

        // 2. Si on a trouvé un blocage, on le joue
        if (found) return;

        // 3. Sinon, choisir stratégiquement
        //    Éviter d'envoyer l'IA dans un bon sous-plateau
        int ordre[][2] = {{1,1}, {0,0}, {0,2}, {2,0}, {2,2}, {0,1}, {1,0}, {1,2}, {2,1}};
        int meilleurScore = -999;
        for (auto& pos : ordre) {
            int r = cibleL + pos[0];
            int c = cibleC + pos[1];
            if (getCase(r, c) == 0) {
                // Où est-ce qu'on envoie l'IA ?
                int envoyeL = (r % 3);
                int envoyeC = (c % 3);
                int score = 0;

                // Bonus si on envoie l'IA dans un sous-plateau condamné (elle joue libre)
                // Malus si on l'envoie dans un sous-plateau qu'elle domine
                if (m_etat[envoyeL][envoyeC] != 0) {
                    score -= 10; // éviter de lui donner le choix libre
                } else {
                    // Compter nos pièces vs les siennes dans le sous-plateau cible
                    int nosP = 0, sesP = 0;
                    for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++) {
                            int val = getCase(envoyeL*3+i, envoyeC*3+j);
                            if (val == 1) nosP++;
                            if (val == -1) sesP++;
                        }
                    score = nosP - sesP; // on préfère l'envoyer où on domine
                }

                // Bonus centre
                if (pos[0] == 1 && pos[1] == 1) score += 3;

                if (score > meilleurScore) {
                    meilleurScore = score;
                    myMove.row = r;
                    myMove.col = c;
                    found = true;
                }
            }
        }
    }

    // Si condamné ou plein, jouer n'importe où
    if (!found) {
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                if (getCase(r, c) == 0 && !estCondamne(r, c)) {
                    myMove.row = r;
                    myMove.col = c;
                    return;
                }
            }
        }
    }
}

bool Plateau::estCondamne(int row, int col) {
    int sousI = row / 3;
    int sousJ = col / 3;
    return m_etat[sousI][sousJ] != 0;
}
