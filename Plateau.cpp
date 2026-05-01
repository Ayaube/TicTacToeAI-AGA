#include <iostream>
#include <array>
#include <vector>
#include <limits>
#include <algorithm>
#include <chrono>
#include "Plateau.h"
#include "main.h"

using namespace std;
using namespace std::chrono;

static const int TIMEOUT_MS = 350;
static time_point<steady_clock> g_debut;

static bool tempsEcoule() {
    return duration_cast<milliseconds>(steady_clock::now() - g_debut).count() >= TIMEOUT_MS;
}

// ============================================================
//  Constructeur
// ============================================================
Plateau::Plateau() {
    for (auto& r : m_g) r.fill(0);
    for (auto& r : m_e) r.fill(0);
}
Plateau::~Plateau() {}

int  Plateau::getCase(int r, int c)          { return m_g[r][c]; }
void Plateau::setCase(int r, int c, int val) { m_g[r][c] = val; }

void Plateau::affiche_plateau() {
    for (auto& row : m_g) {
        for (int v : row) cout << v << " ";
        cout << "\n";
    }
}

// ============================================================
//  Gagnant d'un sous-plateau (offset dL, dC)
// ============================================================
int Plateau::gagnant(int dL, int dC) {
    for (int p : {1, -1}) {
        auto v = [&](int r, int c) { return m_g[dL+r][dC+c] == p; };
        if (v(0,0)&&v(1,1)&&v(2,2)) return p;
        if (v(0,2)&&v(1,1)&&v(2,0)) return p;
        for (int i = 0; i < 3; i++) {
            if (v(i,0)&&v(i,1)&&v(i,2)) return p;
            if (v(0,i)&&v(1,i)&&v(2,i)) return p;
        }
    }
    return 0;
}

void Plateau::verifPlateau() {
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
        if (m_e[i][j] != 0) continue;
        int r = gagnant(i*3, j*3);
        if (r) {
            m_e[i][j] = r;
            cout << (r==1 ? "Joueur" : "IA")
                 << " gagne sous-plateau (" << i << "," << j << ")\n";
        } else if (estPlein(i, j)) {
            // 2 = sous-plateau termine sur un nul
            m_e[i][j] = 2;
        }
    }
}

// ============================================================
//  Helpers
// ============================================================
bool Plateau::estPlein(int si, int sj) {
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++)
        if (m_g[si*3+i][sj*3+j] == 0) return false;
    return true;
}
bool Plateau::estCondamne(int row, int col) {
    int si = row/3, sj = col/3;
    return m_e[si][sj] != 0 || estPlein(si, sj);
}
int Plateau::gagnantMetaGrille() {
    for (int p : {1, -1}) {
        auto e = [&](int i, int j) { return m_e[i][j] == p; };
        if (e(0,0)&&e(1,1)&&e(2,2)) return p;
        if (e(0,2)&&e(1,1)&&e(2,0)) return p;
        for (int i = 0; i < 3; i++) {
            if (e(i,0)&&e(i,1)&&e(i,2)) return p;
            if (e(0,i)&&e(1,i)&&e(2,i)) return p;
        }
    }
    return 0;
}

// ============================================================
//  Simulation
// ============================================================
int Plateau::jouerCoup(int row, int col, int joueur) {
    m_g[row][col] = joueur;
    int si = row/3, sj = col/3;
    int ancien = m_e[si][sj];
    if (ancien == 0) {
        m_e[si][sj] = etatSousPlateau(si, sj);
    }
    return ancien;
}
void Plateau::annulerCoup(int row, int col, int ancienEtat) {
    m_g[row][col] = 0;
    m_e[row/3][col/3] = ancienEtat;
}

int Plateau::etatSousPlateau(int si, int sj) {
    int g = gagnant(si*3, sj*3);
    if (g != 0) return g;
    if (estPlein(si, sj)) return 2;
    return 0;
}

bool Plateau::coupDonneVictoireMeta(const GameMove& move, int joueur) {
    int anc = jouerCoup(move.row, move.col, joueur);
    bool gagne = (gagnantMetaGrille() == joueur);
    annulerCoup(move.row, move.col, anc);
    return gagne;
}

bool Plateau::adversairePeutGagnerMetaAuProchainTour(const GameMove& myMove) {
    int ancMy = jouerCoup(myMove.row, myMove.col, 1);

    GameMove oppBuf[MAX_MOVES];
    int oppN = getCoupsLegauxFast(myMove, oppBuf);
    bool danger = false;

    for (int i = 0; i < oppN; i++) {
        int ancOpp = jouerCoup(oppBuf[i].row, oppBuf[i].col, -1);
        if (gagnantMetaGrille() == -1) {
            danger = true;
            annulerCoup(oppBuf[i].row, oppBuf[i].col, ancOpp);
            break;
        }
        annulerCoup(oppBuf[i].row, oppBuf[i].col, ancOpp);
    }

    annulerCoup(myMove.row, myMove.col, ancMy);
    return danger;
}

// ============================================================
//  Coups legaux : version FAST (buffer, zero malloc)
// ============================================================
int Plateau::getCoupsLegauxFast(const GameMove& last, GameMove buf[MAX_MOVES]) {
    int n = 0;
    bool valide = (last.row>=0 && last.row<9 && last.col>=0 && last.col<9);

    auto ajouter = [&](int si, int sj) {
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++)
            if (m_g[si*3+i][sj*3+j] == 0) buf[n++] = {si*3+i, sj*3+j};
    };

    if (!valide) {
        for (int si = 0; si < 3; si++) for (int sj = 0; sj < 3; sj++)
            if (m_e[si][sj]==0 && !estPlein(si,sj)) ajouter(si,sj);
    } else {
        int ci = last.row%3, cj = last.col%3;
        if (m_e[ci][cj]==0 && !estPlein(ci,cj)) ajouter(ci,cj);
        else for (int si = 0; si < 3; si++) for (int sj = 0; sj < 3; sj++)
            if (m_e[si][sj]==0 && !estPlein(si,sj)) ajouter(si,sj);
    }
    return n;
}

// ============================================================
//  Score rapide d'un coup pour le move ordering
//  (pas de jouerCoup/annulerCoup -> pas de cout)
// ============================================================
static int scorerCoupRapide(const GameMove& c,
                              const array<array<int,9>,9>& g,
                              const array<array<int,3>,3>& e)
{
    int score = 0;
    int si = c.row/3, sj = c.col/3;
    int li = c.row%3, lj = c.col%3;
    int dL = si*3, dC = sj*3;

    // ---- Position locale ----
    if (li==1 && lj==1)              score += 300; // centre local
    else if (li%2==0 && lj%2==0)     score += 150; // coin local

    // ---- Position du sous-plateau dans la meta ----
    if (si==1 && sj==1)              score += 200; // centre meta
    else if (si%2==0 && sj%2==0)     score += 100; // coin meta

    // ---- Menaces locales (ligne, colonne, diagonales) ----
    // Ligne locale li
    {
        int n1=0, nM=0;
        for (int jj=0; jj<3; jj++) {
            int v = g[dL+li][dC+jj];
            if (v== 1) n1++; else if (v==-1) nM++;
        }
        if (n1==2 && nM==0) score += 3000; // on gagne le sous-plateau sur la ligne
        if (nM==2 && n1==0) score += 2500; // on bloque l'adversaire sur la ligne
        if (n1==1 && nM==0) score += 50;
    }
    // Colonne locale lj
    {
        int n1=0, nM=0;
        for (int ii=0; ii<3; ii++) {
            int v = g[dL+ii][dC+lj];
            if (v== 1) n1++; else if (v==-1) nM++;
        }
        if (n1==2 && nM==0) score += 3000;
        if (nM==2 && n1==0) score += 2500;
        if (n1==1 && nM==0) score += 50;
    }
    // Diagonale principale (si applicable)
    if (li == lj) {
        int n1=0, nM=0;
        for (int d=0; d<3; d++) {
            int v = g[dL+d][dC+d];
            if (v== 1) n1++; else if (v==-1) nM++;
        }
        if (n1==2 && nM==0) score += 3000;
        if (nM==2 && n1==0) score += 2500;
    }
    // Anti-diagonale (si applicable)
    if (li+lj == 2) {
        int n1=0, nM=0;
        for (int d=0; d<3; d++) {
            int v = g[dL+d][dC+2-d];
            if (v== 1) n1++; else if (v==-1) nM++;
        }
        if (n1==2 && nM==0) score += 3000;
        if (nM==2 && n1==0) score += 2500;
    }

    // ---- Sous-plateau cible apres ce coup ----
    // On envoie l'adversaire dans le sous-plateau (li, lj)
    int siC = li, sjC = lj;
    // Si ce sous-plateau est deja termine, l'adversaire joue partout -> neutre
    if (e[siC][sjC] != 0) {
        score += 100; // on lui donne la liberte, pas forcement mauvais
    } else {
        // Eviter d'envoyer dans le centre meta (trop fort pour l'adversaire)
        if (siC==1 && sjC==1) score -= 200;
        // Envoyer dans un coin meta = bon (coins sont strategiques)
        if (siC%2==0 && sjC%2==0) score -= 80;
    }

    return score;
}

// ============================================================
//  getCoupsLegaux : version publique avec tri (pour prochainMove)
// ============================================================
vector<GameMove> Plateau::getCoupsLegaux(const GameMove& last) {
    GameMove buf[MAX_MOVES];
    int n = getCoupsLegauxFast(last, buf);
    vector<GameMove> coups(buf, buf+n);
    sort(coups.begin(), coups.end(), [&](const GameMove& a, const GameMove& b) {
        return scorerCoupRapide(a,m_g,m_e) > scorerCoupRapide(b,m_g,m_e);
    });
    return coups;
}

// ============================================================
//  Heuristique d'evaluation
// ============================================================
static inline int scoreLigne(int a, int b, int c, int p) {
    int nP = (a==p)+(b==p)+(c==p);
    int nA = (a==-p)+(b==-p)+(c==-p);
    if (nA > 0) return 0;
    if (nP==3) return 100;
    if (nP==2) return 10;
    if (nP==1) return 1;
    return 0;
}

int Plateau::urgenceMetaGrille(int joueur) {
    static const int L[8][3][2] = {
        {{0,0},{1,1},{2,2}},{{0,2},{1,1},{2,0}},
        {{0,0},{0,1},{0,2}},{{1,0},{1,1},{1,2}},{{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}},{{0,1},{1,1},{2,1}},{{0,2},{1,2},{2,2}}
    };
    int count = 0;
    for (auto& l : L) {
        int nJ=0, nLib=0;
        for (auto& p : l) {
            int v = m_e[p[0]][p[1]];
            if (v==joueur) nJ++; else if (v==0) nLib++;
        }
        if (nJ==2 && nLib==1) count++;
    }
    return count;
}

// Poids strategique d'un sous-plateau (lignes meta encore gagnables)
static int poidsStrategique(int si, int sj,
                             const array<array<int,3>,3>& e, int joueur)
{
    static const int LI[8][3]={{0,1,2},{0,1,2},{0,0,0},{1,1,1},{2,2,2},{0,1,2},{0,1,2},{0,1,2}};
    static const int LJ[8][3]={{0,0,0},{2,1,0},{0,1,2},{0,1,2},{0,1,2},{0,1,2},{1,1,1},{2,2,2}};
    int poids = 1;
    for (int l = 0; l < 8; l++) {
        bool passe = false;
        for (int k = 0; k < 3; k++) if (LI[l][k]==si && LJ[l][k]==sj) { passe=true; break; }
        if (!passe) continue;
        bool ok = true; int nJ = 0;
        for (int k = 0; k < 3; k++) {
            int v = e[LI[l][k]][LJ[l][k]];
            if (v==-joueur) { ok=false; break; }
            if (v==joueur) nJ++;
        }
        if (ok) poids += 1 + nJ*2;
    }
    return poids;
}

static const int BONUS_POS[3][3] = {{4,2,4},{2,7,2},{4,2,4}};

int Plateau::evaluer() {
    int v = gagnantMetaGrille();
    if (v == 1)  return SCORE_VICTOIRE;
    if (v == -1) return SCORE_DEFAITE;

    int score = 0;

    // 1. Alignements sur la meta-grille (tres important)
    for (int p : {1, -1}) {
        int s = 0;
        auto e = [&](int i,int j){ return m_e[i][j]; };
        s += scoreLigne(e(0,0),e(1,1),e(2,2),p)*5;
        s += scoreLigne(e(0,2),e(1,1),e(2,0),p)*5;
        for (int i = 0; i < 3; i++) {
            s += scoreLigne(e(i,0),e(i,1),e(i,2),p)*5;
            s += scoreLigne(e(0,i),e(1,i),e(2,i),p)*5;
        }
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++)
            if (m_e[i][j]==p) s += BONUS_POS[i][j];
        score += p * s * 200;
    }

    // 2. Urgence : menaces meta (defense un peu priorisee)
    score += urgenceMetaGrille( 1) * 650;
    score -= urgenceMetaGrille(-1) * 800;

    // 3. Score local de chaque sous-plateau non termine
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
        if (m_e[i][j] != 0) continue;
        int dL=i*3, dC=j*3;
        for (int p : {1, -1}) {
            int poids = poidsStrategique(i, j, m_e, p);
            int s = 0;
            s += scoreLigne(m_g[dL][dC],  m_g[dL+1][dC+1],m_g[dL+2][dC+2],p);
            s += scoreLigne(m_g[dL][dC+2],m_g[dL+1][dC+1],m_g[dL+2][dC],  p);
            for (int ii=0; ii<3; ii++) {
                s += scoreLigne(m_g[dL+ii][dC],m_g[dL+ii][dC+1],m_g[dL+ii][dC+2],p);
                s += scoreLigne(m_g[dL][dC+ii],m_g[dL+1][dC+ii],m_g[dL+2][dC+ii],p);
            }
            score += p * s * poids;
        }
    }

    return score;
}

// ============================================================
//  Minimax Alpha-Beta SANS allocation dynamique
//  Le tri dans minimax utilise un tableau local (tri par insertion)
// ============================================================
int Plateau::minimax(GameMove last, int depth, int alpha, int beta, int joueur) {
    if (tempsEcoule()) return evaluer();

    int v = gagnantMetaGrille();
    if (v ==  1) return SCORE_VICTOIRE + depth;
    if (v == -1) return SCORE_DEFAITE  - depth;
    if (depth == 0) return evaluer();

    GameMove buf[MAX_MOVES];
    int n = getCoupsLegauxFast(last, buf);
    if (n == 0) return evaluer();

    // Tri par insertion (zero allocation, efficace pour n <= ~20 coups typiques)
    // On ne trie que si depth > 1 pour amortir le cout
    if (depth > 1) {
        int scores[MAX_MOVES];
        for (int k = 0; k < n; k++) scores[k] = scorerCoupRapide(buf[k], m_g, m_e);
        for (int k = 1; k < n; k++) {
            GameMove cm = buf[k]; int cs = scores[k]; int kk = k-1;
            while (kk >= 0 && scores[kk] < cs) {
                buf[kk+1] = buf[kk]; scores[kk+1] = scores[kk]; kk--;
            }
            buf[kk+1] = cm; scores[kk+1] = cs;
        }
    }

    if (joueur == 1) {
        int best = numeric_limits<int>::min();
        for (int k = 0; k < n; k++) {
            if (tempsEcoule()) break;
            int anc = jouerCoup(buf[k].row, buf[k].col, 1);
            int s   = minimax(buf[k], depth-1, alpha, beta, -1);
            annulerCoup(buf[k].row, buf[k].col, anc);
            if (s > best) best = s;
            if (best > alpha) alpha = best;
            if (beta <= alpha) break;
        }
        return best;
    } else {
        int best = numeric_limits<int>::max();
        for (int k = 0; k < n; k++) {
            if (tempsEcoule()) break;
            int anc = jouerCoup(buf[k].row, buf[k].col, -1);
            int s   = minimax(buf[k], depth-1, alpha, beta, 1);
            annulerCoup(buf[k].row, buf[k].col, anc);
            if (s < best) best = s;
            if (best < beta) beta = best;
            if (beta <= alpha) break;
        }
        return best;
    }
}

// ============================================================
//  Point d'entree : approfondissement iteratif
// ============================================================
void Plateau::prochainMove(GameMove& myMove, GameMove& lastMove) {
    vector<GameMove> coups = getCoupsLegaux(lastMove); // tri initial
    int n = (int)coups.size();
    if (n == 0) return;

    // Etape tactique 1:
    // 1) jouer le gain meta immediat s'il existe
    // 2) filtrer les coups qui donnent un gain meta immediat a l'adversaire
    for (const GameMove& c : coups) {
        if (coupDonneVictoireMeta(c, 1)) {
            myMove = c;
            return;
        }
    }

    vector<GameMove> coupsSurs;
    coupsSurs.reserve(coups.size());
    for (const GameMove& c : coups) {
        if (!adversairePeutGagnerMetaAuProchainTour(c)) {
            coupsSurs.push_back(c);
        }
    }
    if (!coupsSurs.empty()) {
        coups.swap(coupsSurs);
        n = (int)coups.size();
    }

    g_debut  = steady_clock::now();
    myMove   = coups[0]; // meilleur par defaut (tri heuristique)

    for (int prof = 1; prof <= PROFONDEUR_MAX; prof++) {
        if (tempsEcoule()) break;

        int bestScore = numeric_limits<int>::min();
        int bestIdx   = 0;
        bool complet  = true;

        for (int k = 0; k < n; k++) {
            if (tempsEcoule()) { complet = false; break; }
            int anc = jouerCoup(coups[k].row, coups[k].col, 1);
            int s   = minimax(coups[k], prof-1,
                              numeric_limits<int>::min(),
                              numeric_limits<int>::max(), -1);
            annulerCoup(coups[k].row, coups[k].col, anc);
            if (s > bestScore) { bestScore = s; bestIdx = k; }
        }

        if (complet) {
            myMove = coups[bestIdx];
            // Remonter le meilleur coup en tete pour la prochaine iteration
            if (bestIdx > 0) swap(coups[0], coups[bestIdx]);
            cerr << "[IDA] prof=" << prof << " score=" << bestScore << "\n";
        }
    }
}
