#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstring>
#include "Plateau.h"
#include "main.h"

using namespace std;

void Plateau::initZobrist() {
    uint64_t s = 0xDEADBEEF12345678ULL;
    auto next = [&]() -> uint64_t { s^=s<<13; s^=s>>7; s^=s<<17; return s; };
    for (int r=0; r<9; r++) for (int c=0; c<9; c++) for (int v=0; v<3; v++)
        m_zobrist[r][c][v] = next();
    for (int i=0; i<10; i++) m_zobristCible[i] = next();
    // Initial hash: all cells are 0, index = 0+1 = 1
    m_hash = 0;
    for (int r=0; r<9; r++) for (int c=0; c<9; c++)
        m_hash ^= m_zobrist[r][c][1];
}

uint64_t Plateau::computeHash(int cibleL, int cibleC) const {
    int ci = (cibleL==-1) ? 0 : 1+(cibleL/3)*3+(cibleC/3);
    return m_hash ^ m_zobristCible[ci];
}

Plateau::Plateau() : m_grille(9, vector<int>(9, 0)), m_etat(3, vector<int>(3, 0)), m_gen(0) {
    initZobrist();
    m_tt = new TTEntry[TT_SIZE]();
}

Plateau::~Plateau() {
    delete[] m_tt;
}

int Plateau::getCase(int row, int col) {
    return m_grille[row][col];
}

void Plateau::setCase(int row, int col, int val) {
    m_hash ^= m_zobrist[row][col][m_grille[row][col]+1];
    m_grille[row][col] = val;
    m_hash ^= m_zobrist[row][col][val+1];
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
        if (getCase(dL, dC) == joueur && getCase(dL+1, dC+1) == joueur && getCase(dL+2, dC+2) == joueur) return joueur;
        if (getCase(dL, dC+2) == joueur && getCase(dL+1, dC+1) == joueur && getCase(dL+2, dC) == joueur) return joueur;
        for (int i = 0; i < 3; i++)
            if (getCase(dL+i, dC) == joueur && getCase(dL+i, dC+1) == joueur && getCase(dL+i, dC+2) == joueur) return joueur;
        for (int j = 0; j < 3; j++)
            if (getCase(dL, dC+j) == joueur && getCase(dL+1, dC+j) == joueur && getCase(dL+2, dC+j) == joueur) return joueur;
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
    int br = r / 3, bc = c / 3;

    if (lr == 1 && lc == 1) score += 4;
    else if (lr != 1 && lc != 1) score += 2;

    static const int L[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };

    for (auto& ligne : L) {
        bool passePar = false;
        for (auto& p : ligne) if (p[0]==lr && p[1]==lc) { passePar=true; break; }
        if (!passePar) continue;
        int nos = 0, adv = 0;
        for (auto& p : ligne) {
            int v = getCase(br*3+p[0], bc*3+p[1]);
            if (v == joueur) nos++; else if (v == -joueur) adv++;
        }
        if (nos == 2) { score += 20; break; }
        if (adv == 2) { score += 15; break; }
    }

    if (estCondamne(lr * 3, lc * 3)) {
        score -= 6;
    } else {
        bool advThreat = false, ourThreat = false;
        for (auto& ligne : L) {
            int nos = 0, adv = 0;
            for (auto& p : ligne) {
                int v = getCase(lr*3+p[0], lc*3+p[1]);
                if (v == joueur) nos++; else if (v == -joueur) adv++;
            }
            if (adv == 2 && nos == 0) advThreat = true;
            if (nos == 2 && adv == 0) ourThreat = true;
        }
        if (advThreat) score -= 10;
        if (ourThreat) score += 12;

        static const int ML[8][3][2] = {
            {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
            {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
            {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
        };
        for (auto& ml : ML) {
            bool dansLigne = false;
            int nosGagnes = 0, euxGagnes = 0;
            for (auto& p : ml) {
                if (p[0]==lr && p[1]==lc) { dansLigne=true; continue; }
                if (m_etat[p[0]][p[1]] == joueur)  nosGagnes++;
                if (m_etat[p[0]][p[1]] == -joueur) euxGagnes++;
            }
            if (!dansLigne) continue;
            if (nosGagnes > 0 && euxGagnes == 0) score += 7 * nosGagnes;
            if (euxGagnes > 0 && nosGagnes == 0) score -= 7 * euxGagnes;
        }
    }

    {
        bool gagneSubBoard = false;
        for (auto& ligne : L) {
            bool passePar = false;
            for (auto& p : ligne) if (p[0]==lr && p[1]==lc) { passePar=true; break; }
            if (!passePar) continue;
            int nos = 0;
            for (auto& p : ligne) {
                int v = getCase(br*3+p[0], bc*3+p[1]);
                if (v == joueur) nos++;
                else if (v == -joueur) { nos=-1; break; }
            }
            if (nos == 2) { gagneSubBoard = true; break; }
        }
        if (gagneSubBoard && m_etat[br][bc] == 0) {
            static const int ML[8][3][2] = {
                {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
                {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
                {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
            };
            m_etat[br][bc] = joueur;
            bool gagneMeta = false;
            for (auto& ml : ML) {
                int v0=m_etat[ml[0][0]][ml[0][1]], v1=m_etat[ml[1][0]][ml[1][1]], v2=m_etat[ml[2][0]][ml[2][1]];
                if ((v0==1||v0==-1) && v0==v1 && v1==v2 && v0==joueur) { gagneMeta=true; break; }
            }
            if (gagneMeta) {
                score += 500;
            } else {
                int menaces = 0;
                for (auto& ml : ML) {
                    int nous=0, eux=0, ouverts=0;
                    for (auto& p : ml) {
                        int e = m_etat[p[0]][p[1]];
                        if (e==joueur) nous++; else if (e==-joueur) eux++; else if (e==0) ouverts++;
                    }
                    if (eux==0 && nous==2 && ouverts==1) menaces++;
                }
                if (menaces >= 2) score += 80;
                else if (menaces == 1) score += 40;
            }
            m_etat[br][bc] = 0;
        }
    }

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
    static const int lignes[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };
    score += getCase(startR + 1, startC + 1) * 3;
    for (auto& ligne : lignes) {
        int nous = 0, eux = 0;
        for (auto& pos : ligne) {
            int val = getCase(startR + pos[0], startC + pos[1]);
            if (val == 1) nous++;
            else if (val == -1) eux++;
        }
        if (eux == 0) { if (nous == 2) score += 10; else if (nous == 1) score += 2; }
        if (nous == 0) { if (eux == 2) score -= 10; else if (eux == 1) score -= 2; }
    }
    return score;
}

int Plateau::evaluerPlateau() {
    int score = 0;

    static const int metaLignes[8][3][2] = {
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
    int nosMenaces = 0, euxMenaces = 0;
    int nosLignesOuvertes = 0, euxLignesOuvertes = 0;
    for (auto& ligne : metaLignes) {
        int nous = 0, eux = 0, ouverts = 0;
        for (auto& pos : ligne) {
            int e = m_etat[pos[0]][pos[1]];
            if (e == 1)       nous++;
            else if (e == -1) eux++;
            else if (e == 0)  ouverts++;
        }
        if (eux == 0) {
            if      (nous == 2 && ouverts == 1) { score += 280; nosMenaces++; }
            else if (nous == 1 && ouverts == 2) { score += 25;  nosLignesOuvertes++; }
            else if (nous == 0 && ouverts == 3)   nosLignesOuvertes++;
        }
        if (nous == 0) {
            if      (eux == 2 && ouverts == 1) { score -= 280; euxMenaces++; }
            else if (eux == 1 && ouverts == 2) { score -= 25;  euxLignesOuvertes++; }
            else if (eux == 0 && ouverts == 3)   euxLignesOuvertes++;
        }
    }
    // Fourchette : 2+ menaces = l'adversaire ne peut pas tout bloquer
    if (nosMenaces >= 2) score += 450;
    if (euxMenaces >= 2) score -= 450;
    // Dominance territoriale : beaucoup de lignes ouvertes = plus d'options stratégiques
    if (nosLignesOuvertes >= 4) score += 35;
    if (euxLignesOuvertes >= 4) score -= 35;

    return score;
}

// Quiescence search : cherche uniquement les coups qui gagnent un sous-plateau
// pour éviter l'effet d'horizon sur les séquences tactiques.
int Plateau::quiesce(int alpha, int beta, bool estMax, int cibleL, int cibleC, int maxPlies) {
    static const int ML[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };
    for (auto& l : ML) {
        int v0=m_etat[l[0][0]][l[0][1]], v1=m_etat[l[1][0]][l[1][1]], v2=m_etat[l[2][0]][l[2][1]];
        if ((v0==1||v0==-1) && v0==v1 && v1==v2)
            return v0==1 ? 2000 : -2000;
    }

    // La qualité du sous-plateau contraint affecte la valeur de la position :
    // si le joueur suivant est forcé dans un sous-plateau qui nous est favorable, bonus.
    int standPat = evaluerPlateau();
    if (cibleL >= 0 && m_etat[cibleL/3][cibleC/3] == 0)
        standPat += evaluerSousPlateau(cibleL, cibleC) * 2;

    if (maxPlies == 0) return standPat;

    if (estMax) {
        if (standPat >= beta) return standPat;
        // Delta pruning : si même un sous-plateau gagné ne suffit pas, on coupe
        if (standPat + 450 <= alpha) return standPat;
        if (standPat > alpha) alpha = standPat;
    } else {
        if (standPat <= alpha) return standPat;
        if (standPat - 450 >= beta) return standPat;
        if (standPat < beta) beta = standPat;
    }

    static const int L[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };

    auto coups = getCoupsValides(cibleL, cibleC);
    int joueur = estMax ? 1 : -1;

    for (auto [r, c] : coups) {
        int lr=r%3, lc=c%3, br=r/3, bc=c/3;
        if (m_etat[br][bc] != 0) continue;

        bool wins = false;
        for (auto& ligne : L) {
            bool passePar = false;
            for (auto& p : ligne) if (p[0]==lr && p[1]==lc) { passePar=true; break; }
            if (!passePar) continue;
            int nos = 0; bool blocked = false;
            for (auto& p : ligne) {
                int v = getCase(br*3+p[0], bc*3+p[1]);
                if (v == joueur) nos++;
                else if (v == -joueur) { blocked=true; break; }
            }
            if (!blocked && nos == 2) { wins = true; break; }
        }
        if (!wins) continue;

        setCase(r, c, joueur);
        int oldEtat = m_etat[r/3][c/3];
        int newEtat = etatSousPlateau(r/3, c/3);
        if (newEtat != 0) m_etat[r/3][c/3] = newEtat;
        int newCibleL=(r%3)*3, newCibleC=(c%3)*3;
        if (estCondamne(newCibleL, newCibleC)) { newCibleL=-1; newCibleC=-1; }

        int score = quiesce(alpha, beta, !estMax, newCibleL, newCibleC, maxPlies - 1);

        setCase(r, c, 0);
        m_etat[r/3][c/3] = oldEtat;

        if (estMax) {
            if (score > alpha) alpha = score;
            if (alpha >= beta) return alpha;
        } else {
            if (score < beta) beta = score;
            if (alpha >= beta) return beta;
        }
    }

    return estMax ? alpha : beta;
}

int Plateau::minimax(int profondeur, int alpha, int beta, bool estMax, int cibleL, int cibleC) {
    static const int ML[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };
    for (auto& l : ML) {
        int v0=m_etat[l[0][0]][l[0][1]], v1=m_etat[l[1][0]][l[1][1]], v2=m_etat[l[2][0]][l[2][1]];
        if ((v0==1||v0==-1) && v0==v1 && v1==v2)
            return v0==1 ? (1000+profondeur) : -(1000+profondeur);
    }

    // Détection de nul méta (tous les sous-plateaux décidés, pas de vainqueur)
    {
        bool allDecided = true;
        for (int i = 0; i < 3 && allDecided; i++)
            for (int j = 0; j < 3 && allDecided; j++)
                if (m_etat[i][j] == 0) allDecided = false;
        if (allDecided) return 0;
    }

    // Horizon → quiescence pour poursuivre les séquences tactiques
    if (profondeur == 0) return quiesce(alpha, beta, estMax, cibleL, cibleC, 3);

    auto coups = getCoupsValides(cibleL, cibleC);
    if (coups.empty()) return quiesce(alpha, beta, estMax, cibleL, cibleC, 3);

    // Futility pruning : position tellement mauvaise qu'aucun coup ne peut la sauver
    if (profondeur == 1) {
        int eval = evaluerPlateau();
        if (estMax  && eval + 220 <= alpha) return eval;
        if (!estMax && eval - 220 >= beta)  return eval;
    }

    // TT lookup — adressage direct O(1), pas de collision de hash map
    uint64_t hash = computeHash(cibleL, cibleC);
    int origAlpha = alpha;
    int8_t tt_r = -1, tt_c = -1;
    {
        TTEntry& e = m_tt[hash & (TT_SIZE - 1)];
        if (e.key == hash && e.gen == m_gen) {
            tt_r = e.best_r; tt_c = e.best_c;
            if (e.depth >= (int16_t)profondeur) {
                if (e.flag==0) return e.score;
                if (e.flag==1) alpha = max(alpha, e.score);
                if (e.flag==2) beta  = min(beta,  e.score);
                if (alpha >= beta) return e.score;
            }
        }
    }

    int joueur = estMax ? 1 : -1;
    int kd = profondeur < 12 ? profondeur : 11;

    // Move ordering: TT move > killer 0 > killer 1 > history + positional heuristic
    auto orderScore = [&](int r, int c) -> int {
        if (r==tt_r && c==tt_c)                                         return 1000000;
        if (r==m_killers[kd][0][0] && c==m_killers[kd][0][1])          return 900000;
        if (r==m_killers[kd][1][0] && c==m_killers[kd][1][1])          return 890000;
        return prioriteCoup(r, c, joueur) * 500 + m_history[r][c];
    };
    sort(coups.begin(), coups.end(), [&](auto& a, auto& b){
        return orderScore(a.first,a.second) > orderScore(b.first,b.second);
    });

    int best = estMax ? -9999 : 9999;
    int8_t best_r = (int8_t)coups[0].first, best_c = (int8_t)coups[0].second;
    int moveIdx = 0;

    for (auto [r, c] : coups) {
        setCase(r, c, joueur);
        int oldEtat = m_etat[r/3][c/3];
        int newEtat = etatSousPlateau(r/3, c/3);
        if (newEtat != 0) m_etat[r/3][c/3] = newEtat;
        int newCibleL = (r%3)*3, newCibleC = (c%3)*3;
        if (estCondamne(newCibleL, newCibleC)) { newCibleL=-1; newCibleC=-1; }

        int score;
        if (moveIdx == 0) {
            // Coup PV : fenêtre pleine, profondeur pleine
            score = minimax(profondeur - 1, alpha, beta, !estMax, newCibleL, newCibleC);
        } else {
            // PVS + LMR pour les coups non-PV
            bool lmr = (moveIdx >= 4 && profondeur >= 3);
            int sd = lmr ? profondeur - 2 : profondeur - 1;

            // Fenêtre nulle : vérifie rapidement si le coup bat alpha/beta
            if (estMax)
                score = minimax(sd, alpha, alpha + 1, false, newCibleL, newCibleC);
            else
                score = minimax(sd, beta - 1, beta, true, newCibleL, newCibleC);

            // Re-recherche à profondeur pleine si c'est prometteur
            bool promising = estMax ? (score > alpha) : (score < beta);
            if (promising)
                score = minimax(profondeur - 1, alpha, beta, !estMax, newCibleL, newCibleC);
        }

        setCase(r, c, 0);
        m_etat[r/3][c/3] = oldEtat;

        if (estMax) {
            if (score > best) { best = score; best_r = r; best_c = c; }
            alpha = max(alpha, best);
        } else {
            if (score < best) { best = score; best_r = r; best_c = c; }
            beta = min(beta, best);
        }

        if (beta <= alpha) {
            // Killer move update (shift slot 0 → slot 1, new move → slot 0)
            if (!(r==m_killers[kd][0][0] && c==m_killers[kd][0][1])) {
                m_killers[kd][1][0]=m_killers[kd][0][0]; m_killers[kd][1][1]=m_killers[kd][0][1];
                m_killers[kd][0][0]=r;                   m_killers[kd][0][1]=c;
            }
            m_history[r][c] += profondeur * profondeur;
            break;
        }
        moveIdx++;
    }

    int flag = (best <= origAlpha) ? 2 : (best >= beta) ? 1 : 0;
    TTEntry& slot = m_tt[hash & (TT_SIZE - 1)];
    // Remplace si: nouvelle position, génération différente, ou profondeur supérieure
    if (slot.key != hash || slot.gen != m_gen || (int16_t)profondeur >= slot.depth)
        slot = {hash, best, (int16_t)profondeur, (uint8_t)flag, m_gen, best_r, best_c};
    return best;
}

void Plateau::prochainMove(GameMove &myMove, GameMove &lastMove) {
    m_gen++;  // invalide les entrées TT de la recherche précédente
    memset(m_killers, -1, sizeof(m_killers));
    memset(m_history, 0, sizeof(m_history));
    int cibleL, cibleC;

    if (lastMove.row == -1) {
        cibleL = -1; cibleC = -1;
    } else {
        cibleL = (lastMove.row % 3) * 3;
        cibleC = (lastMove.col % 3) * 3;
        if (estCondamne(cibleL, cibleC)) { cibleL = -1; cibleC = -1; }
    }

    auto coups = getCoupsValides(cibleL, cibleC);
    if (coups.empty() && cibleL != -1) {
        cibleL = -1; cibleC = -1;
        coups = getCoupsValides(-1, -1);
    }
    if (coups.empty()) return;

    int n = (int)coups.size();
    if (n == 1) { myMove = {coups[0].first, coups[0].second}; return; }

    // Profondeur adaptative selon le nombre de coups disponibles
    int maxDepth;
    if (cibleL == -1)  maxDepth = 5;   // choix libre, branching potentiellement élevé
    else if (n <= 3)   maxDepth = 8;   // sous-plateau presque plein, arbre très étroit
    else if (n <= 5)   maxDepth = 7;   // sous-plateau peu peuplé, encore étroit
    else               maxDepth = 6;   // sous-plateau majoritairement vide
    myMove = {coups[0].first, coups[0].second};

    // Iterative deepening avec réordonnancement complet par score entre itérations
    // et aspiration windows à partir de la profondeur 3
    int prevScore = 0;

    for (int depth = 1; depth <= maxDepth; depth++) {
        vector<int> scores(n);

        // Aspiration window : fenêtre étroite autour du score précédent
        int lo = (depth >= 3) ? prevScore - 80 : -99999;
        int hi = (depth >= 3) ? prevScore + 80 :  99999;

        bool needReSearch = false;
        do {
            needReSearch = false;
            int meilleurScore = -99999;
            int bestIdx = 0;

            for (int i = 0; i < n; i++) {
                auto [r, c] = coups[i];
                setCase(r, c, 1);
                int oldEtat = m_etat[r/3][c/3];
                int newEtat = etatSousPlateau(r/3, c/3);
                if (newEtat != 0) m_etat[r/3][c/3] = newEtat;

                int newCibleL = (r % 3) * 3, newCibleC = (c % 3) * 3;
                if (estCondamne(newCibleL, newCibleC)) { newCibleL = -1; newCibleC = -1; }

                int score = minimax(depth - 1, lo, hi, false, newCibleL, newCibleC);

                setCase(r, c, 0);
                m_etat[r/3][c/3] = oldEtat;

                scores[i] = score;
                if (score > meilleurScore) { meilleurScore = score; bestIdx = i; }
            }

            // Si le score tombe hors fenêtre → relancer avec fenêtre complète
            if (depth >= 3 && (meilleurScore <= lo || meilleurScore >= hi)) {
                lo = -99999; hi = 99999;
                needReSearch = true;
            } else {
                prevScore = meilleurScore;
                myMove = {coups[bestIdx].first, coups[bestIdx].second};
            }
        } while (needReSearch);

        // Réordonner tous les coups par score décroissant pour la prochaine itération
        // (meilleur move ordering = meilleures coupures alpha-beta)
        vector<int> idx(n);
        iota(idx.begin(), idx.end(), 0);
        stable_sort(idx.begin(), idx.end(), [&](int a, int b){ return scores[a] > scores[b]; });
        vector<pair<int,int>> sorted(n);
        for (int i = 0; i < n; i++) sorted[i] = coups[idx[i]];
        coups = sorted;
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
