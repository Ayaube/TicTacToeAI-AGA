# Preparation Soutenance - Ultimate Tic-Tac-Toe IA

Date de travail: 2026-05-02  
Branche de reference: `ayoub_medium2_tt`  
Meilleur etat code connu: `b955909` (`m2 palier3 retour profondeur 9`)  
Meilleure variante mesuree: `step6b` - table de transposition + meilleur coup priorise  
Meilleur score `MEDIUM_2`: `23.16%` hors egalites  
Validation officielle d'un niveau: `80%` de victoires en mode Arena, egalites exclues

Ce document sert de support de preparation pour la soutenance. Il rassemble les attendus du sujet, les choix de conception, les algorithmes, les tests effectues, les regressions, les rollbacks, et les arguments a presenter clairement a l'oral.

---

## 1. Attendus officiels du projet

Source: `Projet IA Ultimate Tic Tac Toe.pdf`.

### Objectif

Le projet consiste a coder une IA en C++ pour jouer a Ultimate Tic-Tac-Toe contre plusieurs IA fournies par l'enseignant.

Niveaux disponibles:

| Famille | Niveaux |
|---|---|
| Faciles | `EASY_1`, `EASY_2` |
| Intermediaires | `MEDIUM_1`, `MEDIUM_2` |
| Difficiles | `HARD_1`, `HARD_2` |
| Tres difficiles | `VERY_HARD_1`, `VERY_HARD_2` |

### Validation d'un niveau

Pour valider un niveau:

- il faut gagner au minimum `80%` des matchs
- la validation se fait uniquement en mode `Arena`
- les egalites ne sont pas comptabilisees dans le pourcentage

Formule:

```text
winrate = victoires / (victoires + defaites)
```

Exemple:

```text
80 victoires, 10 defaites, 10 egalites
winrate = 80 / (80 + 10) = 88.89%
```

### Mode Arena

Le mode Arena est le seul mode de validation.

Caracteristiques:

- 100 parties
- 50 parties avec les croix
- 50 parties avec les ronds
- aleatoire actif
- timeouts calcules

Dans nos tests VM, le mode Arena continue parfois a tourner au-dela de la valeur demandee selon le comportement de la librairie. Notre protocole a donc ete de surveiller les logs et d'arreter le processus a `Start game #100`.

### Mode Debug

Le mode Debug est utile pour tester et demontrer:

- suppression de l'aleatoire
- choix possible du joueur qui commence
- timeouts non bloquants
- affichage des temps pour aider a optimiser

### Timeouts

Le sujet donne les budgets indicatifs suivants:

| Niveau | Timeout indicatif |
|---|---:|
| Facile | 500 ms |
| Intermediaire | 400 ms |
| Difficile | 300 ms |
| Tres difficile | 200 ms |
| Premier coup | 1000 ms |

Le PDF indique ensuite que le systeme de timeout comme sanction automatique a ete retire. Les timeouts restent donc surtout un indicateur d'optimisation. Notre IA garde un budget interne prudent de `350 ms` dans `Plateau.cpp`.

### Contraintes de rendu

- rendu exclusivement via DVL
- deadline indiquee sur DVL
- retard penalise de `2 points par jour`
- code C++ obligatoire
- programmation orientee objet obligatoire
- code en C interdit dans le projet C++
- le code doit compiler et ne pas planter au demarrage
- un code qui ne compile pas ou plante vaut au maximum `10/20`

### Criteres de notation

Le sujet mentionne notamment:

- respect rigoureux des regles du jeu
- qualite du code de l'IA
- performances
- modularite
- bonne repartition des taches

### Oral

Soutenance:

- environ 10 minutes par equipe
- PowerPoint non obligatoire

A preparer:

- presentation rapide des choix de programmation
- structures de donnees utilisees
- conteneurs STL utilises
- optimisations du code
- demonstration live du niveau le plus eleve atteint
- repartition des taches
- conclusion breve: difficultes, bilan collectif et individuel

---

## 2. Regles Ultimate Tic-Tac-Toe respectees

Notre implementation doit insister sur le respect exact des regles du PDF, car le sujet precise qu'il existe plusieurs variantes en ligne.

### Grille

Le plateau est compose de:

- 9 sous-grilles de Tic-Tac-Toe
- chaque sous-grille fait 3x3
- la meta-grille fait 3x3

### Premier coup

Le premier joueur peut jouer n'importe ou:

```text
9 sous-grilles x 9 cases = 81 coups possibles
```

### Redirection

Apres un coup, la position locale du coup dans sa sous-grille determine la sous-grille cible du joueur suivant.

Exemple:

- un coup joue en haut-gauche d'une sous-grille envoie l'adversaire dans la sous-grille haut-gauche de la meta-grille
- un coup joue au centre envoie l'adversaire dans la sous-grille centrale

Dans le code, cette contrainte est geree par `lastMove` et par la generation des coups legaux.

### Sous-grille terminee

Quand une sous-grille est gagnee:

- elle devient terminee
- on ne peut plus jouer dedans

Quand une sous-grille est pleine sans gagnant:

- elle devient aussi terminee
- elle est notee comme nulle localement

Dans le code:

- `m_e[i][j] == 1`: sous-grille gagnee par notre joueur
- `m_e[i][j] == -1`: sous-grille gagnee par l'IA adverse
- `m_e[i][j] == 2`: sous-grille terminee sur nul local
- `m_e[i][j] == 0`: sous-grille encore jouable

### Coup libre

Si le joueur est envoye vers:

- une sous-grille deja terminee
- une sous-grille pleine

alors il peut jouer dans n'importe quelle sous-grille encore disponible.

Dans le code, cette situation est geree dans `getCoupsLegauxFast`.

### Victoire globale

Le gagnant principal est celui qui aligne 3 sous-grilles gagnees:

- horizontalement
- verticalement
- diagonalement

Dans le code, c'est gere par `gagnantMetaGrille()`.

### Fin sans alignement global

Le PDF precise qu'a la fin de la partie, si aucun joueur n'a aligne 3 sous-grilles, alors le joueur qui a gagne le plus de sous-grilles gagne. Sinon, c'est une egalite.

La librairie fournie gere le resultat officiel de la partie. Notre IA doit surtout respecter les coups legaux et l'etat des sous-grilles.

---

## 3. Architecture du code

Fichiers principaux:

| Fichier | Role |
|---|---|
| `main.cpp` | boucle de jeu, lecture configuration, appels librairie, synchronisation plateau |
| `Plateau.h` | declaration de la classe `Plateau` |
| `Plateau.cpp` | logique du jeu, generation des coups, evaluation, minimax, alpha-beta, table de transposition |
| `main.h` | interface fournie avec la librairie |
| `README.md` | commandes, resultats, protocole |
| `HANDOFF-TT.md` | handoff detaille Palier 3 / table de transposition |

### Choix POO

Le coeur du projet est encapsule dans une classe `Plateau`.

Responsabilites de `Plateau`:

- stocker l'etat du plateau
- calculer les coups legaux
- detecter les victoires locales et globales
- simuler et annuler des coups
- evaluer une position
- lancer la recherche minimax
- choisir le prochain coup

Cette organisation respecte l'esprit POO demande par le sujet: la logique du jeu est regroupee dans un objet coherent plutot que dispersee dans `main.cpp`.

### Representation des donnees

Dans `Plateau`:

```cpp
std::array<std::array<int,9>,9> m_g;
std::array<std::array<int,3>,3> m_e;
```

`m_g` represente la grille complete 9x9.

Valeurs:

- `1`: notre joueur
- `-1`: adversaire
- `0`: case vide

`m_e` represente l'etat de la meta-grille.

Valeurs:

- `1`: sous-grille gagnee par nous
- `-1`: sous-grille gagnee par l'adversaire
- `2`: sous-grille pleine / nulle
- `0`: sous-grille encore ouverte

Pourquoi `std::array`:

- taille fixe connue a la compilation
- pas d'allocation dynamique
- acces rapide
- representation simple a expliquer

### Conteneurs STL utilises

| Conteneur | Utilisation | Justification |
|---|---|---|
| `std::array` | plateau 9x9, meta-grille 3x3 | taille fixe, rapide, pas d'allocation |
| `std::vector<GameMove>` | liste de coups racine / interface publique | pratique pour trier et manipuler les coups au niveau racine |
| `std::vector<TTEntry>` | table de transposition globale | tableau de taille fixe alloue une fois |

### Optimisation des allocations

Dans `minimax`, on evite les allocations dynamiques.

Au lieu de retourner un `vector` a chaque noeud, on utilise:

```cpp
GameMove buf[MAX_MOVES];
int n = getCoupsLegauxFast(last, buf);
```

Interet:

- moins d'allocations
- moins de pression memoire
- recursion plus rapide
- plus simple pour tenir le budget de temps

---

## 4. Boucle de jeu et interface librairie

Dans `main.cpp`, le programme:

1. lit la configuration depuis les arguments
2. initialise la librairie
3. cree un `Plateau` pour chaque partie
4. recupere le coup adverse avec `game.getMove`
5. met a jour notre plateau interne
6. demande a `Plateau::prochainMove` de choisir un coup
7. envoie le coup avec `game.setMove`
8. met a jour notre plateau interne

Arguments supportes:

```bash
./UTTT_Template [mode] [level] [nbGames] [alwaysFirst] [alias]
```

Exemples:

```bash
./UTTT_Template arena med1 100 0 AGA
./UTTT_Template arena med2 100 0 AGA
./UTTT_Template debug med2 10 1 AGA
```

Niveaux geres dans `main.cpp`:

- `easy1`
- `easy2`
- `med1`
- `med2`
- `hard1`
- `hard2`
- `vhard1`
- `vhard2`

---

## 5. Algorithme de decision

### Vue d'ensemble

Notre IA utilise une approche de recherche adversariale classique:

```text
Generation des coups legaux
-> garde-fous tactiques racine
-> approfondissement iteratif
-> minimax
-> alpha-beta pruning
-> evaluation heuristique
-> table de transposition
-> tri des coups
```

### Minimax

Principe:

- notre joueur cherche a maximiser le score
- l'adversaire cherche a minimiser le score
- on explore l'arbre des coups possibles jusqu'a une profondeur limitee ou jusqu'au timeout interne

Dans le code:

```cpp
int minimax(GameMove last, int depth, int alpha, int beta, int joueur);
```

`joueur == 1`:

- on maximise

`joueur == -1`:

- on minimise

### Alpha-beta

Alpha-beta permet d'eviter d'explorer des branches qui ne peuvent plus changer la decision finale.

Interet:

- reduire fortement le nombre de noeuds explores
- atteindre une profondeur plus elevee dans le meme temps

Dans les compteurs TT instrumentes, on observe un taux de coupures au premier coup de `74.55%`, ce qui montre que le tri des coups aide bien l'alpha-beta.

### Approfondissement iteratif

`prochainMove` teste progressivement les profondeurs:

```text
profondeur 1
profondeur 2
...
PROFONDEUR_MAX
```

Avantages:

- si le temps s'ecoule, on garde le meilleur coup completement calcule a la profondeur precedente
- le meilleur coup trouve a une profondeur est remonte en tete pour l'iteration suivante
- cela ameliore le tri des coups

Valeur actuelle:

```cpp
PROFONDEUR_MAX = 9
```

Nous avons teste `PROFONDEUR_MAX = 11`, mais cela a regresse.

### Garde-fous tactiques racine

Avant de lancer toute la recherche, la version stable conserve deux garde-fous simples:

1. si un coup gagne immediatement la meta-grille, on le joue
2. on filtre les coups qui permettent a l'adversaire de gagner immediatement la meta-grille au prochain tour

Ces garde-fous viennent de `step1` et ont donne un gain statistique important.

Nous avons evite de les supprimer apres plusieurs regressions observees sur les variantes qui tentaient de laisser toute la decision a l'evaluation continue.

---

## 6. Fonction d'evaluation

La fonction `evaluer()` donne un score a une position quand la recherche s'arrete.

Idee generale:

- score positif: position favorable pour nous
- score negatif: position favorable pour l'adversaire
- victoire globale: score tres eleve
- defaite globale: score tres negatif

Scores terminaux:

```cpp
SCORE_VICTOIRE = 1000000
SCORE_DEFAITE  = -1000000
```

Elements pris en compte:

- alignements sur la meta-grille
- sous-grilles deja gagnees
- menaces a deux sous-grilles alignees
- score local des sous-grilles encore ouvertes
- poids strategique des sous-grilles selon leur place dans la meta-grille

Positionnement strategique:

- centre plus important
- coins importants
- bords moins importants

Reflexion importante:

Nous avons tente plusieurs retunings agressifs de l'evaluation. La plupart ont regresse contre `MEDIUM_2`. Conclusion: l'evaluation actuelle n'est pas parfaite, mais elle est fragile; les changements de poids doivent etre testes tres prudemment.

---

## 7. Tri des coups

Le tri des coups est crucial avec alpha-beta.

### Tri rapide

Chaque coup recoit un score rapide via `scorerCoupRapide`.

Il valorise notamment:

- centre local
- coins locaux
- position de la sous-grille dans la meta-grille
- menaces locales
- blocages locaux
- sous-grille cible envoyee a l'adversaire

### Tri sans allocation dans minimax

Dans `minimax`, les coups sont tries par insertion dans un tableau local.

Pourquoi insertion sort:

- peu de coups typiquement
- tableau local
- pas d'allocation
- cout faible

### Meilleur coup TT priorise

La meilleure version `step6b` ajoute un bonus tres fort au meilleur coup venant de la table de transposition.

Effet mesure avec `TT_STATS`:

- TT bestMove legal / known: `96.09%`
- TT bestMove premier apres tri / legal: `99.97%`

Conclusion: dans la version actuelle, la TT agit surtout comme outil de move ordering.

---

## 8. Table de transposition

### Objectif

La table de transposition memorise des positions deja evaluees pendant la recherche.

Elle sert a:

- eviter certains recalculs
- recuperer un score si l'entree est assez profonde
- recuperer un meilleur coup pour ameliorer le tri

### Structure

```cpp
struct TTEntry {
    uint64_t key;
    int depth;
    int score;
    TTFlag flag;
    GameMove bestMove;
    unsigned int generation;
};
```

Champs:

| Champ | Role |
|---|---|
| `key` | hash de l'etat |
| `depth` | profondeur de recherche associee |
| `score` | score memorise |
| `flag` | type d'entree: exact, borne basse, borne haute |
| `bestMove` | meilleur coup trouve depuis cet etat |
| `generation` | generation courante de recherche |

Taille actuelle:

```cpp
TT_SIZE = 1 << 20
```

### Flags

| Flag | Signification |
|---|---|
| `EXACT` | score exact |
| `BORNE_BASSE` | score est une borne basse |
| `BORNE_HAUTE` | score est une borne haute |
| `VIDE` | entree inutilisee |

### Hash

La version stable utilise un hash recalcule a partir de:

- grille 9x9
- meta-grille 3x3
- dernier coup `lastMove`
- joueur courant

Nous avons teste un hash incremental, mais il a regresse. Nous sommes donc revenus au hash recalcule.

### Generation

La generation TT est incrementee a chaque `prochainMove`.

Cela signifie:

- la table est utile pendant l'approfondissement iteratif d'un coup donne
- elle n'est pas conservee entre deux coups differents de notre IA

Nous avons teste la conservation entre coups. Cela a regresse.

---

## 9. Protocole de test

### Environnement VM

Les benchmarks officiels ont ete faits sur la VM Windows `win-dev`.

Chemin VM principal:

```text
C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA
```

Pour eviter d'ecraser des modifications locales sur la VM, les benchmarks recents ont ete faits dans des clones propres dedies:

```text
TicTacToeAI-AGA-bench-052513c
TicTacToeAI-AGA-bench-29a617e
TicTacToeAI-AGA-bench-665f856
TicTacToeAI-AGA-bench-5b4980e
TicTacToeAI-AGA-stats-6da4b6e
```

### Build VM

Commande qui fonctionne:

```bash
g++ -std=c++17 -O2 -fno-lto main.cpp Plateau.cpp -L. -lUTTTLib allegro_font-5.2.dll allegro_ttf-5.2.dll allegro_image-5.2.dll allegro_primitives-5.2.dll allegro-5.2.dll -o ia_local.exe
```

Pourquoi `-fno-lto`:

- sans cette option, la compilation echoue a cause d'un mismatch LTO avec `libUTTTLib.a`

### Execution VM

Execution directe par SSH:

```text
Failed to create display!
```

Cause:

- Allegro a besoin d'une session graphique interactive

Solution:

- lancer via une tache planifiee interactive Windows
- utiliser `schtasks /IT`
- rediriger la sortie vers un log
- surveiller `Start game #`
- tuer le processus a 100 parties pour les benchmarks standards

### Parsing des logs

Commandes locales utilisees:

```bash
starts=$(rg -c "Start game #" run.log)
wins=$(rg -c "Game: Winner .*\\(PLAYER\\)" run.log)
losses=$(rg -c "Game: Winner .*\\(IA\\)" run.log)
draws=$(rg -c "IA AND PLAYER|NO_WINNER" run.log)
```

Winrate:

```bash
wins / (wins + losses)
```

---

## 10. Resultats consolides

### Resultat MEDIUM_1

| Niveau | Variante | Parties | Victoires | Defaites | Egalites | Winrate hors egalites | Validation |
|---|---|---:|---:|---:|---:|---:|---|
| `MEDIUM_1` | base `ayoub` | 100 | 98 | 0 | 2 | 100.00% | OK |

Conclusion:

- `MEDIUM_1` est valide
- le projet atteint au moins le niveau intermediaire 1

### Resultats MEDIUM_2

| Variante | Idee | Parties | Victoires | Defaites | Egalites | Winrate hors egalites | Decision |
|---|---|---:|---:|---:|---:|---:|---|
| baseline `ayoub` | base avant travail MEDIUM_2 | 100 | 15 | 83 | 2 | 15.31% | base insuffisante |
| `step1` | tactique racine simple | 100 | 21 | 76 | 3 | 21.65% | gardee comme base stable |
| `step2` | filtre / tri tactique racine | 100 | 17 | 81 | 1 | 17.35% | rollback |
| `step3` | evaluation anyboard + centre/coins | 100 | 13 | 83 | 4 | 13.54% | rollback |
| `step3b` | ANY_BOARD seul | 100 | 15 | 81 | 3 | 15.62% | rollback |
| `step4` | evaluation continue meta anyboard | 100 | 13 | 83 | 3 | 13.54% | rollback |
| `step4a` | anyboard doux | 100 | 14 | 81 | 5 | 14.74% | rollback |
| `step4b` | defense meta | 100 | 11 | 83 | 6 | 11.70% | rollback |
| `step5` | killer moves simple | 100 | 12 | 84 | 3 | 12.50% | rollback |
| `step6` | table de transposition simple | 100 | 21 | 75 | 3 | 21.88% | petit gain |
| `step6b` | TT + meilleur coup priorise | 100 | 22 | 73 | 5 | 23.16% | meilleur etat |
| `step6c` | profondeur max 11 | 100 | 21 | 74 | 5 | 22.11% | rollback |
| `step7` | TT conservee entre coups | 100 | 19 | 75 | 6 | 20.21% | rollback |
| `step8` | hash incremental | 100 | 20 | 70 | 10 | 22.22% | rollback |
| `step9` | remplacement TT profondeur d'abord | 100 | 14 | 78 | 8 | 15.22% | rollback |
| `step10` | tri racine par scores TT | 100 | 18 | 77 | 5 | 18.95% | rollback |

Meilleur score actuel contre `MEDIUM_2`:

```text
23.16%
```

Conclusion:

- `MEDIUM_2` n'est pas valide
- l'objectif 80% reste tres loin
- mais le travail a permis d'ameliorer la baseline `15.31%` vers `23.16%`
- les tests sont documentes et les regressions ont ete rollbackees

---

## 11. Historique des essais et reflexions

### Baseline `ayoub`

Score `MEDIUM_2`:

```text
15.31%
```

Etat:

- IA deja fonctionnelle
- minimax + alpha-beta + evaluation
- bonne base contre `MEDIUM_1`
- insuffisante contre `MEDIUM_2`

Conclusion:

- il fallait ameliorer la robustesse tactique et l'efficacite de recherche

### Step1 - tactique racine simple

Score:

```text
21.65%
```

Idee:

- ajouter des garde-fous simples a la racine
- jouer une victoire globale immediate si possible
- eviter les coups qui donnent une victoire globale immediate a l'adversaire

Resultat:

- gain net par rapport a la baseline
- meilleure base avant la table de transposition

Interpretation:

- certaines erreurs contre `MEDIUM_2` etaient tactiques et immediates
- des garde-fous simples peuvent aider plus que des retunings complexes

Decision:

- garde dans les versions suivantes

### Step2 - filtre / tri tactique racine

Score:

```text
17.35%
```

Idee:

- renforcer le tri tactique racine
- essayer de mieux choisir parmi les coups candidats

Resultat:

- regression nette

Interpretation:

- trop forcer la racine peut perturber minimax
- une heuristique locale peut masquer une meilleure sequence profonde

Decision:

- rollback

### Step3 - evaluation ANY_BOARD + centre/coins

Score:

```text
13.54%
```

Idee:

- ameliorer l'evaluation statique
- valoriser centre et coins
- ajouter une logique autour du coup libre `ANY_BOARD`

Resultat:

- forte regression

Interpretation:

- les poids heuristiques sont tres sensibles
- penaliser ou valoriser le coup libre trop fortement peut conduire a des choix artificiels

Decision:

- rollback

### Step3b - ANY_BOARD seul

Score:

```text
15.62%
```

Idee:

- isoler l'effet `ANY_BOARD`
- verifier si la regression venait du centre/coins ou du coup libre

Resultat:

- legerement mieux que step3 mais toujours sous step1

Interpretation:

- le bonus/malus ANY_BOARD seul n'apporte pas assez
- l'evaluation de la mobilite est delicate dans UTTT

Decision:

- rollback

### Step4 - evaluation continue meta anyboard

Score:

```text
13.54%
```

Idee:

- laisser minimax choisir via des scores continus
- reduire l'impact des filtres racine
- renforcer l'evaluation globale

Resultat:

- regression forte

Interpretation:

- contrairement a l'intuition theorique, supprimer les garde-fous racine a degrade les performances
- `MEDIUM_2` punit probablement les erreurs tactiques immediates

Decision:

- rollback

### Step4a - ANY_BOARD doux

Score:

```text
14.74%
```

Idee:

- rendre le bonus/malus de coup libre moins agressif

Resultat:

- regression toujours nette

Interpretation:

- meme en version douce, cette piste n'a pas apporte de gain

Decision:

- rollback

### Step4b - defense meta

Score:

```text
11.70%
```

Idee:

- renforcer la defense sur la meta-grille
- mieux bloquer les menaces globales

Resultat:

- pire resultat parmi les variantes Palier 1

Interpretation:

- trop defendre peut rendre l'IA passive
- les poids defensifs peuvent sacrifier l'initiative

Decision:

- rollback

### Step5 - Killer Moves simple

Score:

```text
12.50%
```

Idee:

- utiliser une heuristique classique de move ordering
- memoriser des coups provoquant des coupures beta

Resultat:

- regression forte

Interpretation:

- l'implementation simple des killer moves etait probablement trop peu contextualisee pour UTTT
- un coup fort dans une sous-grille ne reste pas forcement fort ailleurs a cause de la redirection

Decision:

- rollback

### Step6 - table de transposition simple

Score:

```text
21.88%
```

Idee:

- ajouter une table de transposition simple
- memoriser score, profondeur, flag, meilleur coup

Resultat:

- petit gain par rapport a step1

Interpretation:

- Palier 3 est la premiere piste qui donne un signal positif apres step1

Decision:

- continuer dans cette direction

### Step6b - table de transposition + meilleur coup priorise

Score:

```text
23.16%
```

Idee:

- utiliser le `bestMove` stocke dans la TT pour prioriser le tri des coups

Resultat:

- meilleur score obtenu contre `MEDIUM_2`

Interpretation:

- la TT est surtout utile pour ordonner les coups
- meilleur coup TT + alpha-beta donne un gain mesurable

Decision:

- meilleur etat conserve

### Step6c - profondeur max 11

Score:

```text
22.11%
```

Idee:

- augmenter `PROFONDEUR_MAX` de 9 a 11
- tenter de profiter de la TT pour chercher plus loin

Resultat:

- regression legere

Interpretation:

- plus profond n'est pas automatiquement meilleur
- avec le budget temps, profondeur 11 peut produire plus de recherches incompletes ou de mauvais timings

Decision:

- rollback a profondeur 9

### Step7 - conserver la TT entre coups

Score:

```text
20.21%
```

Idee:

- ne plus invalider la TT a chaque `prochainMove`
- conserver les positions explorees entre deux coups de notre IA

Resultat:

- regression

Interpretation:

- les entrees anciennes peuvent polluer le tri ou les bornes
- la politique de remplacement simple n'est pas assez robuste pour une TT persistante

Decision:

- rollback

### Step8 - hash incremental

Score:

```text
22.22%
```

Idee:

- remplacer le scan complet du plateau dans `hashEtat` par un hash maintenu incrementalement
- garder la meme formule de cle pour limiter les risques

Validation locale:

- test temporaire comparant hash incremental et recomputation complete: OK

Resultat:

- proche du meilleur score, mais inferieur a step6b

Interpretation:

- le hash etait valide techniquement
- gagner du CPU ne suffit pas si cela modifie le timing ou la distribution des profondeurs atteintes

Decision:

- rollback

### Step9 - remplacement TT profondeur d'abord

Score:

```text
15.22%
```

Idee:

- ne remplacer une entree TT existante que si la nouvelle recherche est au moins aussi profonde

Resultat:

- forte regression

Interpretation:

- garder une entree profonde mais moins pertinente peut nuire au moteur
- le remplacement simple s'accorde mieux avec l'etat actuel de la recherche

Decision:

- rollback

### Step10 - tri racine avec scores TT

Score:

```text
18.95%
```

Idee:

- utiliser les scores TT des enfants pour trier les coups racine a l'iteration suivante

Resultat:

- regression

Interpretation:

- le simple swap du meilleur coup apres chaque iteration est meilleur
- trier toute la racine par score TT peut sur-biaiser certains choix

Decision:

- rollback

---

## 12. Instrumentation TT

Apres plusieurs essais TT regressifs, nous avons ajoute une instrumentation pour comprendre plutot que tester a l'aveugle.

### Activation

Instrumentation active uniquement avec:

```bash
-DTT_STATS
```

Build instrumente:

```bash
g++ -std=c++17 -O2 -fno-lto -DTT_STATS main.cpp Plateau.cpp -L. -lUTTTLib allegro_font-5.2.dll allegro_ttf-5.2.dll allegro_image-5.2.dll allegro_primitives-5.2.dll allegro-5.2.dll -o ia_stats.exe
```

Le build normal n'est pas cense etre impacte.

### Compteurs mesures

- noeuds visites
- probes TT
- hits TT
- hits utilisables
- flags exact / borne basse / borne haute
- stores
- collisions
- meilleur coup TT connu
- meilleur coup TT legal
- meilleur coup TT premier apres tri
- coupures alpha-beta
- coupures au premier coup
- profondeur moyenne des hits
- profondeur complete atteinte par coup

### Run de reference

Log:

```text
run_med2_ttstats_20.log
```

Contexte:

- `MEDIUM_2`
- Arena court instrumente
- environ 20 parties completes
- 515 lignes `[TT]`

### Synthese

| Metrique | Valeur |
|---|---:|
| Probes TT | 3,189,652 |
| Hits TT | 871,198 |
| Hit rate | 27.31% |
| Hits utilisables | 34,099 |
| Usable / hits | 3.91% |
| Usable / probes | 1.07% |
| Exact returns | 937 |
| Lower-bound returns | 28,959 |
| Upper-bound returns | 4,203 |
| Stores | 3,145,574 |
| Collisions | 7,972 |
| Collision rate | 0.25% |
| TT bestMove legal / known | 96.09% |
| TT bestMove first / legal | 99.97% |
| First-move cutoff rate | 74.55% |
| Avg hit depth | 1.51 |
| Avg completed depth | 6.48 |

### Interpretation

Ce que les chiffres montrent:

- la TT est bien consultee
- elle touche assez souvent: `27.31%` de hit rate
- les collisions sont faibles: `0.25%`
- le meilleur coup TT est tres souvent legal: `96.09%`
- quand il est legal, il est presque toujours premier apres tri: `99.97%`
- les coupures au premier coup sont bonnes: `74.55%`

Mais:

- seulement `3.91%` des hits sont utilisables pour retourner un score
- seulement `1.07%` des probes retournent directement un score

Conclusion:

La TT actuelle sert surtout a ameliorer l'ordre des coups. Elle ne sert pas beaucoup a eviter directement le recalcul d'un sous-arbre via un score memorise.

Implication pour la suite:

- agrandir la table n'est pas prioritaire car les collisions sont faibles
- le meilleur coup TT est deja tres fortement priorise
- le vrai sujet est le faible taux de hits utilisables

---

## 13. Ce qui a fonctionne

### Garde-fous tactiques racine

Impact:

```text
15.31% -> 21.65%
```

Pourquoi c'est important:

- evite des erreurs tactiques immediates
- protege contre les victoires globales adverses au prochain coup
- simple a expliquer

### Table de transposition simple

Impact:

```text
21.65% -> 21.88%
```

Petit gain, mais signal positif.

### Meilleur coup TT priorise

Impact:

```text
21.88% -> 23.16%
```

Meilleur resultat obtenu.

### Approfondissement iteratif + swap du meilleur coup

Cette strategie reste utile:

- elle garde un meilleur coup valide meme en cas de timeout interne
- elle ameliore naturellement l'ordre a la profondeur suivante
- elle a mieux marche que le tri racine par scores TT

---

## 14. Ce qui n'a pas fonctionne

### Retuning agressif de l'evaluation

Plusieurs variantes ont regresse:

- `step3`
- `step3b`
- `step4`
- `step4a`
- `step4b`

Conclusion:

- l'evaluation est sensible
- les poids intuitifs ne suffisent pas
- contre `MEDIUM_2`, une evaluation plus complexe peut empirer les choix

### Suppression ou affaiblissement des garde-fous racine

Les variantes laissant davantage minimax trancher via evaluation continue ont regresse.

Conclusion:

- les garde-fous simples restent utiles
- la theorie pure ne suffit pas si l'evaluation n'est pas assez fiable

### Killer Moves simple

Score:

```text
12.50%
```

Conclusion:

- trop peu contextualise pour UTTT
- la redirection rend les coups moins universels qu'aux echecs

### Profondeur max 11

Score:

```text
22.11%
```

Conclusion:

- chercher plus profond ne suffit pas
- le moteur doit completer ses iterations dans le budget temps

### TT persistante entre coups

Score:

```text
20.21%
```

Conclusion:

- garder des entrees anciennes pollue probablement la recherche
- la generation par `prochainMove` est plus stable

### Hash incremental

Score:

```text
22.22%
```

Conclusion:

- techniquement correct
- pas meilleur statistiquement
- rollback par prudence

### Remplacement TT profondeur d'abord

Score:

```text
15.22%
```

Conclusion:

- garder des entrees profondes n'est pas forcement meilleur
- une entree profonde peut etre moins pertinente pour la branche courante

### Tri racine par scores TT

Score:

```text
18.95%
```

Conclusion:

- a fortement biaise la racine
- le swap simple du meilleur coup est meilleur

---

## 15. Message cle pour la soutenance

Le message a faire passer:

```text
Nous avons construit une IA basee sur minimax, alpha-beta et approfondissement iteratif.
Nous avons beaucoup teste contre la VM, en Arena, avec 100 parties par variante.
Nous n'avons pas garde les optimisations juste parce qu'elles semblaient intelligentes:
nous avons rollbacke tout ce qui regressait statistiquement.
La meilleure amelioration vient de la table de transposition utilisee comme aide au tri des coups.
```

Ce message est important parce qu'il montre:

- une demarche experimentale
- une capacite a mesurer
- une capacite a revenir en arriere
- une bonne comprehension des compromis

---

## 16. Proposition de plan oral en 10 minutes

### 0:00 - 1:00 : Introduction

- presenter rapidement Ultimate Tic-Tac-Toe
- expliquer que le jeu est plus complexe que le morpion classique
- rappeler l'objectif: battre les IA du prof et valider des niveaux a 80% en Arena

Phrase possible:

```text
Notre objectif etait de construire une IA C++ orientee objet capable de jouer legalement et efficacement a Ultimate Tic-Tac-Toe, puis de la tester en mode Arena contre les niveaux fournis.
```

### 1:00 - 2:00 : Regles gerees

Montrer que l'IA respecte:

- redirection par position locale du dernier coup
- coup libre si sous-grille cible terminee ou pleine
- sous-grille gagnee non rejouable
- victoire globale par alignement de sous-grilles

Phrase possible:

```text
On a fait attention a respecter les regles exactes du sujet, car Ultimate Tic-Tac-Toe a plusieurs variantes. Notre generation de coups part toujours du dernier coup joue et verifie si la sous-grille cible est encore disponible.
```

### 2:00 - 3:00 : Architecture C++

Presenter:

- `main.cpp`
- `Plateau.h/.cpp`
- `std::array` pour la grille
- `std::vector` seulement la ou c'est utile
- buffers fixes dans minimax

Phrase possible:

```text
La classe Plateau concentre la logique du jeu: etat, coups legaux, simulation, evaluation et recherche. Pour la performance, la recursion utilise des tableaux fixes plutot que des vectors alloues a chaque noeud.
```

### 3:00 - 5:00 : Algorithme IA

Presenter:

- minimax
- alpha-beta
- approfondissement iteratif
- evaluation heuristique
- tri des coups
- garde-fous tactiques racine

Phrase possible:

```text
Notre IA explore l'arbre des coups avec minimax et alpha-beta. Comme le temps est limite, on utilise un approfondissement iteratif: on garde toujours le meilleur coup de la derniere profondeur terminee.
```

### 5:00 - 6:30 : Optimisations

Presenter:

- zero allocation dans `minimax`
- tri rapide des coups
- table de transposition
- meilleur coup TT priorise
- instrumentation TT

Phrase possible:

```text
La table de transposition nous a surtout aide comme outil de move ordering. Les mesures montrent 27% de hits, mais seulement 3.9% de hits directement utilisables comme score. En revanche, le meilleur coup TT est presque toujours legal et premier apres tri.
```

### 6:30 - 8:00 : Resultats et iterations

Montrer le tableau des scores.

Message principal:

- `MEDIUM_1` valide a 100% hors egalites
- `MEDIUM_2` baseline 15.31%
- meilleur `MEDIUM_2` 23.16%
- beaucoup de variantes ont ete rollbackees

Phrase possible:

```text
On a adopte une demarche experimentale: chaque optimisation a ete testee sur 100 parties Arena. Si elle regressait, on la documentait puis on revenait au meilleur etat connu.
```

### 8:00 - 9:00 : Demonstration live

Niveau conseille:

- demontrer `MEDIUM_1`, car il est valide
- eventuellement montrer `MEDIUM_2` pour expliquer que c'est le niveau de travail actuel mais non valide

A montrer:

- lancement en Arena ou Debug
- coups joues par l'IA
- logs `[IDA] prof=... score=...`

Phrase possible:

```text
Pour la demonstration, on lance le niveau le plus eleve que nous validons de maniere fiable. On peut aussi montrer MEDIUM_2 pour illustrer les limites actuelles et la recherche en cours.
```

### 9:00 - 10:00 : Conclusion

A dire:

- difficulte principale: l'evaluation et les optimisations ne progressent pas toujours intuitivement
- bilan positif: IA fonctionnelle, modulaire, testee, MEDIUM_1 valide
- bilan technique: table de transposition utile mais surtout pour le tri
- perspectives: ameliorer le taux de hits utilisables ou travailler une evaluation plus fiable

Phrase possible:

```text
La principale difficulte a ete que beaucoup d'optimisations classiques ne marchent pas automatiquement sur Ultimate Tic-Tac-Toe. Le jeu est tres sensible au contexte cree par les redirections. Notre progression vient surtout des tests systematiques et des rollbacks.
```

---

## 17. Demonstration live conseillee

### Option sure

Montrer `MEDIUM_1`.

Pourquoi:

- valide a 100% hors egalites sur notre run de reference
- bonne demonstration de fonctionnement
- moins de risque de perdre vite devant le jury

Commande type:

```bash
./UTTT_Template arena med1 100 0 AGA
```

Ou sur VM:

```bat
ia_local.exe arena med1 100 0 AGA
```

### Option ambitieuse

Montrer `MEDIUM_2` en expliquant:

- niveau non valide
- score actuel 23.16%
- niveau utilise pour nos recherches d'optimisation

Commande:

```bat
ia_local.exe arena med2 100 0 AGA
```

### Debug pour expliquer

Le mode Debug est utile pour expliquer sans aleatoire:

```bat
ia_local.exe debug med2 10 1 AGA
```

A montrer si possible:

- coups envoyes
- profondeur atteinte `[IDA]`
- score de recherche

---

## 18. Repartition des taches

A completer avec les noms reels de l'equipe.

Proposition de structure:

| Membre | Contributions |
|---|---|
| Membre 1 | architecture `Plateau`, regles du jeu, generation des coups legaux |
| Membre 2 | minimax, alpha-beta, evaluation heuristique |
| Membre 3 | tests VM, benchmarks Arena, documentation des resultats |
| Membre 4 | optimisations: tri des coups, table de transposition, instrumentation |

Si l'equipe est plus petite, regrouper les lignes.

Important a dire:

```text
Nous avons separe les responsabilites entre logique du jeu, algorithme de decision, tests et documentation, mais les choix finaux ont ete valides collectivement avec les benchmarks.
```

---

## 19. Difficultes rencontrees

### Complexite du jeu

Ultimate Tic-Tac-Toe a un facteur de branchement important.

Difficulte:

- impossible de tout explorer
- necessite une evaluation approximative
- la redirection rend les coups tres contextuels

### Evaluation fragile

Plusieurs ameliorations intuitives ont regresse.

Exemples:

- poids centre/coins
- bonus/malus coup libre
- defense meta agressive

Lecon:

- en IA de jeu, une heuristique doit etre validee statistiquement
- plus complexe ne veut pas dire meilleur

### Optimisations classiques pas toujours adaptees

Killer Moves, profondeur plus grande, TT persistante, remplacement profondeur d'abord: toutes ces idees sont classiques, mais elles ont regresse ici.

Lecon:

- UTTT est tres contextuel
- les optimisations doivent etre adaptees au jeu

### Environnement de test

Difficulte VM:

- execution directe SSH impossible a cause d'Allegro
- besoin de `schtasks /IT`
- besoin de surveiller et tuer le processus
- logs a parser proprement

Lecon:

- le protocole de test est aussi important que le code

---

## 20. Bilan

### Bilan collectif

Points positifs:

- IA fonctionnelle
- respect des regles
- code structure en C++ POO
- `MEDIUM_1` valide
- progression contre `MEDIUM_2`
- protocole de benchmark fiable
- documentation des essais et regressions

Limites:

- `MEDIUM_2` non valide
- score encore loin de 80%
- evaluation difficile a ameliorer
- plusieurs optimisations classiques non concluantes

### Bilan technique

Ce qui a vraiment aide:

- garde-fous tactiques racine
- alpha-beta avec bon tri des coups
- approfondissement iteratif
- table de transposition pour prioriser le meilleur coup

Ce qui a moins aide:

- retuning agressif de l'evaluation
- Killer Moves simple
- profondeur 11
- TT persistante
- hash incremental simple
- politique TT profondeur d'abord
- tri racine par scores TT

### Perspectives

Pistes futures raisonnables:

- mieux comprendre pourquoi les hits TT sont peu utilisables
- analyser les flags TT et profondeurs de stockage
- ameliorer l'evaluation avec tests unitaires de positions tactiques
- construire une petite suite de positions critiques au lieu de se baser uniquement sur Arena
- travailler un move ordering plus contextuel pour UTTT

---

## 21. Phrases utiles pour questions du jury

### Pourquoi pas MCTS ?

```text
Nous avons choisi de rester sur minimax/alpha-beta car le projet etait deja structure autour d'une evaluation heuristique et d'une recherche adversariale. Passer a MCTS aurait implique une reecriture importante et plus de complexite pour un gain incertain dans le temps du projet.
```

### Pourquoi garder des heuristiques qui semblent simples ?

```text
Parce qu'elles ont ete validees statistiquement. Les garde-fous racine simples ont donne un gain net contre MEDIUM_2, alors que plusieurs heuristiques plus sophistiquees ont regresse.
```

### Pourquoi rollback des optimisations ?

```text
Nous avons decide qu'une optimisation devait etre gardee uniquement si elle ameliorait le score Arena. Sinon, meme si elle etait interessante theoriquement, on la documentait et on revenait au meilleur etat connu.
```

### Pourquoi la table de transposition aide peu en score direct ?

```text
Nos compteurs montrent que la table a un hit rate correct, mais peu d'entrees sont assez profondes pour retourner directement un score. En pratique, elle aide surtout a ordonner les coups avec le meilleur coup memorise.
```

### Pourquoi MEDIUM_2 reste difficile ?

```text
MEDIUM_2 semble punir fortement les erreurs tactiques et les mauvais choix de redirection. Beaucoup d'optimisations augmentent la complexite ou changent le timing sans ameliorer la qualite reelle des decisions.
```

---

## 22. Checklist avant rendu

A verifier avant depot DVL:

- le projet compile sur l'environnement cible
- les DLL Allegro sont presentes
- `libUTTTLib.a` est present
- le code ne depend pas de fichiers temporaires
- l'instrumentation `TT_STATS` n'est pas activee dans le build rendu sauf si explicitement voulu
- les logs temporaires ne sont pas necessaires au rendu
- le niveau de demonstration est choisi
- la repartition des taches est completee avec les vrais noms
- le meilleur commit/code est bien celui de `step6b` + instrumentation inactive par defaut

---

## 23. Resume ultra-court

```text
Nous avons developpe une IA C++ orientee objet pour Ultimate Tic-Tac-Toe.
Elle respecte les regles du sujet, utilise minimax, alpha-beta, approfondissement iteratif, evaluation heuristique, tri des coups et table de transposition.
MEDIUM_1 est valide avec 100% hors egalites.
Sur MEDIUM_2, la baseline etait a 15.31% et notre meilleur score est 23.16%.
Nous avons teste de nombreuses optimisations, mais nous avons rollbacke celles qui regressaient.
La meilleure amelioration vient de la table de transposition utilisee pour prioriser le meilleur coup.
```
