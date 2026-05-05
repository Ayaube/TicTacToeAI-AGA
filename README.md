# TicTacToeAI-AGA — Ultimate Tic-Tac-Toe IA

Projet de POO — ESILV  
Groupe : AGA  
Objectif : implémenter un bot jouant au **Ultimate Tic-Tac-Toe (UTTT)** en C++, capable de battre les niveaux imposés par le sujet.

---

## Résultats obtenus

| Niveau    | Parties | Victoires | Défaites | Nuls |
|-----------|---------|-----------|----------|------|
| MEDIUM_1  | 100     | 100       | 0        | 0    |

Log conservé dans `test-logs/run_med1_soutenance_100done.log`.

---

## Règles du jeu (Ultimate Tic-Tac-Toe)

Le plateau est une grille **9×9** divisée en **9 sous-grilles 3×3**.  
Chaque tour, un joueur place son symbole dans une cellule.  
**La règle de redirection** : la colonne de la cellule jouée dans la sous-grille détermine **dans quelle sous-grille le prochain coup doit être joué**.  
Si cette sous-grille cible est déjà terminée (gagnée ou pleine), le joueur peut jouer n'importe où.  
Le gagnant est celui qui aligne 3 sous-grilles sur la **méta-grille** (la grille des gagnants des sous-grilles).

---

## Architecture du projet

```
TicTacToeAI-AGA/
├── main.h          # Interface fournie par le prof (ne pas modifier)
├── main.cpp        # Boucle de jeu — point d'entrée
├── Plateau.h       # Déclaration de la classe Plateau + constantes IA
├── Plateau.cpp     # Implémentation complète : plateau + IA
└── libUTTTLib.a    # Bibliothèque du moteur de jeu (fournie)
```

---

## Description détaillée de chaque fichier

---

### `main.h` — Interface du moteur de jeu (fourni)

Ce fichier est **fourni par le professeur** et ne doit pas être modifié. Il définit le contrat entre notre IA et le moteur de jeu externe.

#### Énumérations

```cpp
enum Level { EASY_1, EASY_2, MEDIUM_1, MEDIUM_2, HARD_1, HARD_2, VERY_HARD_1, VERY_HARD_2 };
enum Mode  { DEBUG, ARENA };
```

- `Level` : niveau de difficulté de l'adversaire simulé par le moteur.
- `Mode::DEBUG` : affichage graphique du plateau (fenêtre Allegro).
- `Mode::ARENA` : mode silencieux pour les campagnes de test automatisées.

#### Structure `GameMove`

```cpp
struct GameMove {
    int row; // ligne  (0..8)
    int col; // colonne (0..8)
};
```

Représente une case de la grille 9×9. C'est le format universel d'échange de coups avec le moteur.

#### Énumération `Winner`

```cpp
enum Winner { NO_WINNER, IA, PLAYER, IA_AND_PLAYER };
```

Retournée par `game.getWinner()` à la fin de chaque partie.

#### Interface abstraite `IGame`

```cpp
class IGame {
public:
    virtual void initialize(int nbGames, Level, Mode, bool swapSides, std::string teamName) = 0;
    virtual bool isGameFinished() = 0;
    virtual bool isMatchFinished() = 0;
    virtual bool getMove(GameMove&) = 0;   // récupère le dernier coup adverse
    virtual void setMove(GameMove) = 0;    // envoie notre coup au moteur
    virtual Winner getWinner() = 0;
};
extern IGame& game; // instance globale du moteur, fournie par libUTTTLib.a
```

Notre code interagit **exclusivement** via ces méthodes. Le moteur gère l'affichage, la validation des coups et le score.

---

### `main.cpp` — Boucle de jeu (point d'entrée)

Ce fichier orchestre les parties. Il est volontairement **minimal** : toute l'intelligence est dans `Plateau`.

#### Initialisation

```cpp
game.initialize(10, Level::MEDIUM_1, Mode::ARENA, false, "AGA");
```

| Paramètre   | Valeur       | Signification                              |
|-------------|--------------|---------------------------------------------|
| `nbGames`   | 10           | Nombre de parties à jouer                  |
| `Level`     | MEDIUM_1     | Niveau de l'adversaire                     |
| `Mode`      | ARENA        | Pas de fenêtre graphique                   |
| `swapSides` | false        | On joue toujours en premier                |
| `teamName`  | "AGA"        | Nom de l'équipe affiché dans les logs      |

#### Boucle principale

```
tant que le match n'est pas fini :
    tant que la partie n'est pas finie :
        getMove()         → récupère le coup adverse (bloquant)
        prochainMove()    → notre IA calcule la réponse
        setMove()         → envoie notre coup au moteur
```

La variable `gameMove` (dernier coup adverse) est transmise à `Plateau::prochainMove()` pour que l'IA connaisse la sous-grille cible imposée par la règle de redirection.

Un `Plateau` est réinstancié à chaque nouvelle partie via `plateau = Plateau()`, garantissant un état propre.

---

### `Plateau.h` — Déclaration de la classe Plateau

#### Constantes globales

```cpp
const int SCORE_VICTOIRE = 1000000;   // score terminal gagnant
const int SCORE_DEFAITE  = -1000000;  // score terminal perdant
const int PROFONDEUR_MAX = 9;         // profondeur maximale de recherche
const int MAX_MOVES      = 81;        // nombre maximal de coups légaux possibles
```

Ces valeurs sont des **sentinelles** : toute évaluation non terminale doit rester strictement entre `SCORE_DEFAITE` et `SCORE_VICTOIRE` pour que l'algorithme ne confonde pas un coup "bon mais non gagnant" avec une victoire garantie.

#### Données membres privées

```cpp
int m_g[9][9];   // grille 9×9 : 0 = vide, 1 = IA, -1 = adversaire
int m_e[3][3];   // méta-grille : état de chaque sous-grille (même convention)
```

La convention de signe **+1 / -1** permet d'inverser la perspective sans branchement conditionnel dans l'évaluation (`score * joueur`).

#### Interface publique principale

| Méthode | Rôle |
|---|---|
| `getCase(r,c)` / `setCase(r,c,v)` | Accès aux cellules de la grille |
| `getEtat()` | Retourne la méta-grille |
| `affiche_plateau()` | Affichage debug en console |
| `gagnant(dL, dC)` | Vérifie si la sous-grille (dL,dC) est gagnée |
| `verifPlateau()` | Met à jour la méta-grille après un coup |
| `estCondamne(r,c)` | Vrai si la case ne peut plus changer le résultat |
| `estPlein(si,sj)` | Vrai si la sous-grille est complète |
| `gagnantMetaGrille()` | Vérifie si quelqu'un a gagné la méta-grille |
| `getCoupsLegaux(last)` | Retourne les coups légaux triés heuristiquement |
| `prochainMove(myMove, lastMove)` | **Point d'entrée principal de l'IA** |

---

### `Plateau.cpp` — Implémentation complète

C'est le cœur du projet. Il contient toute la logique du jeu et de l'IA.

---

#### 1. Gestion du plateau

##### `gagnant(dL, dC)`

Vérifie si la sous-grille dont le coin supérieur gauche est `(dL*3, dC*3)` est gagnée.  
Teste les 8 combinaisons : 3 lignes, 3 colonnes, 2 diagonales.  
Retourne `1`, `-1`, ou `0`.

##### `verifPlateau()`

Parcourt toutes les sous-grilles et met à jour `m_e[i][j]` :
- Si gagnée → valeur du gagnant (`±1`)
- Si pleine sans gagnant → valeur spéciale `2` (nulle)
- Sinon → `0` (encore en jeu)

##### `estCondamne(row, col)`

Une case est "condamnée" si sa sous-grille est déjà terminée (`m_e[sr][sc] != 0`).  
Utilisée pour filtrer les coups dans la génération de coups légaux.

##### `jouerCoup(row, col, joueur)` / `annulerCoup(row, col)`

Play/undo pour la recherche minimax. `annulerCoup` remet la case à `0` et relance `verifPlateau()` pour restaurer `m_e`.

---

#### 2. Génération des coups légaux

##### `getCoupsLegauxFast(buf, last, joueur)`

Version optimisée **sans allocation dynamique** (tableau sur la pile).  
Implémente les règles UTTT :

```
si last.row != -1 :
    sous-grille cible = (last.row % 3, last.col % 3)
    si cette sous-grille n'est pas terminée :
        lister uniquement les cases vides de cette sous-grille
    sinon :
        lister toutes les cases vides de toutes les sous-grilles non terminées
sinon (premier coup) :
    lister toutes les cases vides
```

##### `getCoupsLegaux(last, joueur)`

Appelle `getCoupsLegauxFast`, puis trie les coups par score heuristique décroissant via `scorerCoupRapide`. Retourne un `std::vector<GameMove>`.

---

#### 3. Heuristique de tri des coups (`scorerCoupRapide`)

Évalue un coup **sans le jouer**, pour guider la recherche en explorant d'abord les coups prometteurs (move ordering).

| Critère | Bonus |
|---|---|
| Case centrale d'une sous-grille (position 4) | +3000 |
| Coin d'une sous-grille | +300 à +150 |
| Sous-grille centrale de la méta-grille | +200 |
| Coin de la méta-grille | +100 |
| Coup gagnant la sous-grille localement | +3000 |
| Coup bloquant la victoire adverse en sous-grille | +2500 |
| Coup envoyant l'adversaire dans la sous-grille centrale | −500 (pénalité) |
| Coup envoyant l'adversaire dans un coin | +150 |

Le move ordering est crucial pour l'efficacité de l'alpha-beta : un bon tri peut réduire l'arbre exploré de **O(b^d)** à **O(b^(d/2))**.

---

#### 4. Fonction d'évaluation statique (`evaluer(joueur)`)

Utilisée quand on atteint la profondeur maximale ou un état terminal.

**États terminaux** (court-circuit immédiat) :
```
méta-grille gagnée par joueur    → +SCORE_VICTOIRE
méta-grille gagnée par adversaire → +SCORE_DEFAITE
```

**Évaluation positionnelle** (si partie non terminée) :

1. **Alignements sur la méta-grille** : pour chaque ligne, colonne, diagonale de `m_e`, compter les sous-grilles contrôlées. Une ligne avec 2 sous-grilles contrôlées et une libre vaut beaucoup plus qu'une ligne avec 1.

2. **Menaces sur la méta-grille** (urgence) : détecter les situations "2 sous-grilles alignées + 1 libre" → bonus tactique pour attaquer ou bloquer.

3. **Force locale des sous-grilles** : évaluer chaque sous-grille encore en jeu selon ses alignements internes, pondéré par l'importance stratégique de la sous-grille sur la méta-grille (centre > coins > bords).

4. **Bonus de position** : valeur statique des cases occupées selon leur position (centre et coins valent plus que les bords).

Le score final est multiplié par `joueur` (±1) pour retourner toujours le score **du point de vue du joueur maximisant**.

---

#### 5. Algorithme minimax avec élagage alpha-beta (`minimax`)

```cpp
int minimax(GameMove last, int profondeur, int alpha, int beta, int joueur,
            std::chrono::steady_clock::time_point debut)
```

**Principes** :

- **Minimax** : le joueur `1` (IA) maximise le score, le joueur `-1` (adversaire) le minimise.
- **Alpha-beta** : coupe les branches inutiles. `alpha` = meilleur score garanti pour le maximisant, `beta` = meilleur score garanti pour le minimisant. Si `alpha >= beta` → coupe (le branche ne sera jamais choisie).
- **Limite de temps** : vérifié à chaque nœud via `std::chrono::steady_clock`. Budget : **350 ms** par coup.
- **Tri interne** : les coups sont triés par score heuristique avant expansion (insertion sort, sans allocation dynamique).
- **Conditions d'arrêt** : profondeur 0, état terminal, ou timeout.

Pseudo-code simplifié :
```
minimax(last, profondeur, alpha, beta, joueur):
    si temps écoulé ou profondeur == 0 ou état terminal:
        retourner evaluer(joueur)
    
    pour chaque coup légal (trié par heuristique):
        jouerCoup(coup)
        score = -minimax(coup, profondeur-1, -beta, -alpha, -joueur)
        annulerCoup(coup)
        
        alpha = max(alpha, score)
        si alpha >= beta: break  // élagage
    
    retourner alpha
```

---

#### 6. Approfondissement itératif (`prochainMove`)

```cpp
void prochainMove(GameMove& myMove, GameMove lastMove)
```

C'est le **point d'entrée principal** appelé depuis `main.cpp` à chaque tour.

**Algorithme** :

```
pour profondeur de 1 à PROFONDEUR_MAX (9):
    lancer minimax à cette profondeur
    si timeout pendant la recherche: arrêter
    mémoriser le meilleur coup trouvé
    utiliser l'ordre des coups de cette itération pour la suivante

retourner le meilleur coup trouvé
```

**Avantages de l'approfondissement itératif** :
- Respecte strictement le budget temps (on ne dépasse jamais 350 ms).
- Le meilleur coup d'une profondeur `d` sert d'ordre de tri pour la profondeur `d+1` → améliore l'efficacité de l'alpha-beta à chaque itération.
- Si le temps est très court, on a toujours un coup valide (profondeur 1 au minimum).

---

## Compilation et exécution

### Compiler

```bash
g++ -std=c++17 -O2 -fno-lto main.cpp Plateau.cpp -L. -lUTTTLib \
    allegro_font-5.2.dll allegro_ttf-5.2.dll allegro_image-5.2.dll \
    allegro_primitives-5.2.dll allegro-5.2.dll \
    -o ia_soutenance.exe
```

### Lancer une campagne (100 parties, MEDIUM_1, mode silencieux)

```bash
ia_soutenance.exe arena med1 100 0 AGA
```

### Lancer en mode debug (affichage graphique, 1 partie)

```bash
ia_soutenance.exe debug med1 1 0 AGA
```

---

## Points clés à retenir pour la soutenance

| Concept | Ce qu'on a fait |
|---|---|
| **Encapsulation** | La classe `Plateau` encapsule état + logique, `main.cpp` n'accède jamais directement à `m_g` |
| **Abstraction** | `IGame` est une interface pure — notre code ne connaît pas les détails du moteur |
| **Polymorphisme** | `game` est une référence vers `IGame`, l'implémentation est dans `libUTTTLib.a` |
| **Performance** | Zéro allocation dynamique dans le chemin critique (pile uniquement) |
| **Robustesse** | Budget temps de 350 ms garanti par `steady_clock` à chaque nœud |
| **Qualité algo** | Minimax + alpha-beta + move ordering + approfondissement itératif |
