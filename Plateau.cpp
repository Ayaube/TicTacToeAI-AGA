#include "Plateau.h"
#include <iostream>
#include <algorithm>
#include <limits>

using namespace std;
using namespace std::chrono;

static const int TEMPS_MAX_MS = 1500;  // les timeouts sont desactives par le prof
static const int PROF_MAX = 12;

static const int POIDS_META[3][3] = {
    {3, 2, 3},
    {2, 5, 2},   // centre encore plus important
    {3, 2, 3}
};

// ============================================================
//  Constructeur
// ============================================================

Plateau::Plateau() {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            m_grille[i][j] = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m_etat[i][j] = 0;
}

// ============================================================
//  Interface publique
// ============================================================

void Plateau::jouerIA(int row, int col) {
    m_grille[row][col] = -1;
    mettreAJourEtat(row / 3, col / 3);
}

void Plateau::jouerNous(int row, int col) {
    m_grille[row][col] = 1;
    mettreAJourEtat(row / 3, col / 3);
}

void Plateau::afficher() const {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++)
            cout << m_grille[i][j] << " ";
        cout << "\n";
    }
}

// ============================================================
//  Utilitaires
// ============================================================

bool Plateau::estPlein(int si, int sj) const {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (m_grille[si*3+i][sj*3+j] == 0) return false;
    return true;
}

bool Plateau::estJouable(int si, int sj) const {
    return m_etat[si][sj] == 0 && !estPlein(si, sj);
}

int Plateau::gagnantSous(int si, int sj) const {
    int dL = si*3, dC = sj*3;
    for (int j : {1, -1}) {
        // Diagonales
        if (m_grille[dL][dC]==j && m_grille[dL+1][dC+1]==j && m_grille[dL+2][dC+2]==j) return j;
        if (m_grille[dL][dC+2]==j && m_grille[dL+1][dC+1]==j && m_grille[dL+2][dC]==j) return j;
        // Lignes
        for (int i = 0; i < 3; i++)
            if (m_grille[dL+i][dC]==j && m_grille[dL+i][dC+1]==j && m_grille[dL+i][dC+2]==j) return j;
        // Colonnes
        for (int k = 0; k < 3; k++)
            if (m_grille[dL][dC+k]==j && m_grille[dL+1][dC+k]==j && m_grille[dL+2][dC+k]==j) return j;
    }
    return 0;
}

int Plateau::gagnantMeta() const {
    for (int j : {1, -1}) {
        if (m_etat[0][0]==j && m_etat[1][1]==j && m_etat[2][2]==j) return j;
        if (m_etat[0][2]==j && m_etat[1][1]==j && m_etat[2][0]==j) return j;
        for (int i = 0; i < 3; i++)
            if (m_etat[i][0]==j && m_etat[i][1]==j && m_etat[i][2]==j) return j;
        for (int k = 0; k < 3; k++)
            if (m_etat[0][k]==j && m_etat[1][k]==j && m_etat[2][k]==j) return j;
    }
    return 0;
}

void Plateau::mettreAJourEtat(int si, int sj) {
    if (m_etat[si][sj] != 0) return;
    int g = gagnantSous(si, sj);
    if (g != 0)
        m_etat[si][sj] = g;
    else if (estPlein(si, sj))
        m_etat[si][sj] = 2; // nul
}

// ============================================================
//  Coups légaux
// ============================================================

vector<GameMove> Plateau::coupsLegaux(const GameMove& dernierCoup) const {
    vector<GameMove> coups;
    bool libre = true;

    if (dernierCoup.row >= 0 && dernierCoup.row < 9 &&
        dernierCoup.col >= 0 && dernierCoup.col < 9) {
        int ci = dernierCoup.row % 3;
        int cj = dernierCoup.col % 3;
        if (estJouable(ci, cj)) {
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if (m_grille[ci*3+i][cj*3+j] == 0)
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
                            if (m_grille[si*3+i][sj*3+j] == 0)
                                coups.push_back({si*3+i, sj*3+j});
    }

    // Move ordering : critere principal = ou j'envoie l'adversaire
    sort(coups.begin(), coups.end(), [this](const GameMove& a, const GameMove& b) {
        auto score = [this](const GameMove& m) {
            int s = 0;
            int li = m.row % 3, ci = m.col % 3;

            // Position dans la sous-grille (centre > coin > bord)
            if (li == 1 && ci == 1) s += 4;
            else if (li % 2 == 0 && ci % 2 == 0) s += 2;
            else s += 1;

            // Sous-plateau cible (li, ci)
            int cibleI = li, cibleJ = ci;

            // MAUVAIS : envoyer en libre choix
            if (m_etat[cibleI][cibleJ] != 0 || estPlein(cibleI, cibleJ))
                s -= 8;

            // BON : envoyer dans un sous-plateau ou l'adversaire est mal place
            // (heuristique simple : on regarde l'eval locale)
            else
                s += POIDS_META[cibleI][cibleJ];

            // Bonus si on joue dans un sous-plateau strategique
            int monI = m.row / 3, monJ = m.col / 3;
            s += POIDS_META[monI][monJ];

            return s;
        };
        return score(a) > score(b);
    });

    return coups;
}

// ============================================================
//  Simulation (jouer / annuler sans toucher à l'état réel)
// ============================================================

int Plateau::simulerJouer(int row, int col, int joueur) {
    m_grille[row][col] = joueur;
    int si = row/3, sj = col/3;
    int ancien = m_etat[si][sj];
    if (ancien == 0) {
        int g = gagnantSous(si, sj);
        if (g != 0)      m_etat[si][sj] = g;
        else if (estPlein(si, sj)) m_etat[si][sj] = 2;
    }
    return ancien;
}

void Plateau::simulerAnnuler(int row, int col, int ancienEtat) {
    m_grille[row][col] = 0;
    m_etat[row/3][col/3] = ancienEtat;
}

// ============================================================
//  Évaluation
// ============================================================

// Score d'une ligne de 3 cases dans un sous-plateau
int Plateau::scoreLigneSous(int a, int b, int c, int joueur) const {
    int nJ = (a==joueur)  + (b==joueur)  + (c==joueur);
    int nA = (a==-joueur) + (b==-joueur) + (c==-joueur);
    if (nA > 0) return 0; // bloquée
    if (nJ == 2) return 10;
    if (nJ == 1) return 1;
    return 0;
}

int Plateau::scoreLigneMeta(int a, int b, int c, int joueur) const {
    auto val = [&](int v) { return (v == 2) ? 0 : v; };
    int va = val(a), vb = val(b), vc = val(c);
    int nJ = (va==joueur)  + (vb==joueur)  + (vc==joueur);
    int nA = (va==-joueur) + (vb==-joueur) + (vc==-joueur);
    if (nA > 0 && nJ > 0) return 0;
    if (nA > 0) return 0;
    if (nJ == 3) return 10000; // ligne meta gagnee (devrait pas arriver vu gagnantMeta)
    if (nJ == 2) return 80;    // menace serieuse
    if (nJ == 1) return 12;
    return 0;
}

// Score local d'un sous-plateau non terminé
int Plateau::evaluerSous(int si, int sj) const {
    int dL = si*3, dC = sj*3;
    int score = 0;
    for (int joueur : {1, -1}) {
        int s = 0;
        // Diagonales
        s += scoreLigneSous(m_grille[dL][dC], m_grille[dL+1][dC+1], m_grille[dL+2][dC+2], joueur);
        s += scoreLigneSous(m_grille[dL][dC+2], m_grille[dL+1][dC+1], m_grille[dL+2][dC], joueur);
        // Lignes
        for (int i = 0; i < 3; i++)
            s += scoreLigneSous(m_grille[dL+i][dC], m_grille[dL+i][dC+1], m_grille[dL+i][dC+2], joueur);
        // Colonnes
        for (int j = 0; j < 3; j++)
            s += scoreLigneSous(m_grille[dL][dC+j], m_grille[dL+1][dC+j], m_grille[dL+2][dC+j], joueur);
        score += joueur * s;
    }
    return score;
}

int Plateau::evaluer() const {
    // 1. Fin de partie
    int v = gagnantMeta();
    if (v ==  1) return  100000;
    if (v == -1) return -100000;

    int score = 0;

    // 2. Sous-plateaux gagnes : tres gros bonus + position
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            if (m_etat[i][j] ==  1) score += 80 + POIDS_META[i][j] * 15;
            else if (m_etat[i][j] == -1) score -= 80 + POIDS_META[i][j] * 15;
        }

    // 3. Alignements sur la meta-grille (le critere DECISIF)
    for (int joueur : {1, -1}) {
        int s = 0;
        auto e = [&](int i, int j) { return m_etat[i][j]; };
        s += scoreLigneMeta(e(0,0), e(1,1), e(2,2), joueur);
        s += scoreLigneMeta(e(0,2), e(1,1), e(2,0), joueur);
        for (int i = 0; i < 3; i++)
            s += scoreLigneMeta(e(i,0), e(i,1), e(i,2), joueur);
        for (int j = 0; j < 3; j++)
            s += scoreLigneMeta(e(0,j), e(1,j), e(2,j), joueur);
        score += joueur * s;
    }

    // 4. Detection fork meta : 2 lignes meta a 2-non-bloquees = quasi gagne
    for (int joueur : {1, -1}) {
        int menacesDoubles = 0;
        auto e = [&](int i, int j) { return m_etat[i][j]; };
        auto compte = [&](int a, int b, int c) {
            int va = (a == 2) ? 0 : a;
            int vb = (b == 2) ? 0 : b;
            int vc = (c == 2) ? 0 : c;
            int nJ = (va==joueur) + (vb==joueur) + (vc==joueur);
            int nA = (va==-joueur) + (vb==-joueur) + (vc==-joueur);
            if (nA == 0 && nJ == 2) menacesDoubles++;
        };
        compte(e(0,0), e(1,1), e(2,2));
        compte(e(0,2), e(1,1), e(2,0));
        for (int i = 0; i < 3; i++) compte(e(i,0), e(i,1), e(i,2));
        for (int j = 0; j < 3; j++) compte(e(0,j), e(1,j), e(2,j));
        if (menacesDoubles >= 2) score += joueur * 400;
    }

    // 5. Score local de chaque sous-plateau encore ouvert
    for (int si = 0; si < 3; si++)
        for (int sj = 0; sj < 3; sj++)
            if (m_etat[si][sj] == 0)
                score += evaluerSous(si, sj) * POIDS_META[si][sj];

    // 6. Bonus pour centre absolu et centres de sous-plateaux
    //    (cases tres importantes car elles creent beaucoup de lignes)
    if (m_grille[4][4] ==  1) score += 12;
    if (m_grille[4][4] == -1) score -= 12;

    return score;
}

// ============================================================
//  Minimax avec Alpha-Bêta
// ============================================================

int Plateau::minimax(GameMove dernierCoup, int profondeur,
                     int alpha, int beta, int joueur,
                     steady_clock::time_point deadline) {

    // Timeout : on retourne l'évaluation statique
    if (steady_clock::now() >= deadline)
        return evaluer();

    // Fin de partie
    int v = gagnantMeta();
    if (v ==  1) return  90000 + profondeur; // victoire rapide = meilleure
    if (v == -1) return -90000 - profondeur;

    vector<GameMove> coups = coupsLegaux(dernierCoup);
    if (coups.empty() || profondeur == 0)
        return evaluer();

    if (joueur == 1) {
        // Notre tour : maximiser
        int best = numeric_limits<int>::min();
        for (const GameMove& c : coups) {
            if (steady_clock::now() >= deadline) break;
            int ancien = simulerJouer(c.row, c.col, 1);
            int s = minimax(c, profondeur-1, alpha, beta, -1, deadline);
            simulerAnnuler(c.row, c.col, ancien);
            best = max(best, s);
            alpha = max(alpha, best);
            if (beta <= alpha) break; // coupure
        }
        return best;
    } else {
        // Tour adverse : minimiser
        int best = numeric_limits<int>::max();
        for (const GameMove& c : coups) {
            if (steady_clock::now() >= deadline) break;
            int ancien = simulerJouer(c.row, c.col, -1);
            int s = minimax(c, profondeur-1, alpha, beta, 1, deadline);
            simulerAnnuler(c.row, c.col, ancien);
            best = min(best, s);
            beta = min(beta, best);
            if (beta <= alpha) break; // coupure
        }
        return best;
    }
}

// ============================================================
//  Point d'entrée : approfondissement itératif
// ============================================================

void Plateau::choisirCoup(GameMove& out, const GameMove& dernierCoupAdverse) {
    vector<GameMove> coups = coupsLegaux(dernierCoupAdverse);
    if (coups.empty()) return;

    // Raccourci 1 : coup gagnant immediat
    for (const GameMove& c : coups) {
        int ancien = const_cast<Plateau*>(this)->simulerJouer(c.row, c.col, 1);
        int g = gagnantMeta();
        const_cast<Plateau*>(this)->simulerAnnuler(c.row, c.col, ancien);
        if (g == 1) { out = c; cerr << "[Direct] coup gagnant\n"; return; }
    }
    // Raccourci 2 : bloquer victoire adverse immediate
    for (const GameMove& c : coups) {
        int ancien = const_cast<Plateau*>(this)->simulerJouer(c.row, c.col, -1);
        int g = gagnantMeta();
        const_cast<Plateau*>(this)->simulerAnnuler(c.row, c.col, ancien);
        if (g == -1) { out = c; cerr << "[Direct] blocage forcé\n"; return; }
    }

    // Deadline globale pour ce coup
    auto debut    = steady_clock::now();
    auto deadline = debut + milliseconds(TEMPS_MAX_MS);

    out = coups[0]; // coup par défaut si timeout immédiat

    // Approfondissement itératif : profondeur 1, 2, 3 ... PROF_MAX
    for (int prof = 1; prof <= PROF_MAX; prof++) {
        if (steady_clock::now() >= deadline) break;

        int bestScore = numeric_limits<int>::min();
        GameMove bestCoup = coups[0];
        bool complet = true;

        for (const GameMove& c : coups) {
            if (steady_clock::now() >= deadline) { complet = false; break; }
            int ancien = simulerJouer(c.row, c.col, 1);
            int s = minimax(c, prof-1,
                            numeric_limits<int>::min(),
                            numeric_limits<int>::max(),
                            -1, deadline);
            simulerAnnuler(c.row, c.col, ancien);
            if (s > bestScore) { bestScore = s; bestCoup = c; }
        }

        if (complet) {
            out = bestCoup;
            cerr << "[Minimax] prof=" << prof << " score=" << bestScore
                 << " coup=(" << out.row << "," << out.col << ")\n";
        }
    }
}
