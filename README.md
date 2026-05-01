# Ultimate Tic-Tac-Toe - Projet IA (ESILV A3)

## Objectif
Coder une IA en C++ (POO) capable d'affronter les IA du sujet sur plusieurs niveaux:

- `EASY_1`, `EASY_2`
- `MEDIUM_1`, `MEDIUM_2`
- `HARD_1`, `HARD_2`
- `VERY_HARD_1`, `VERY_HARD_2`

Validation d'un niveau: au moins 80% de victoires en mode Arena (hors egalites).

## Etat de la branche `ayoub_medium2`

Cette branche sert aux tests iteratifs pour battre `MEDIUM_2`:

- base issue de `ayoub`
- changements appliques par etapes (un commit par etape)
- objectif: progresser sans ajouter de complexite inutile

Points en place:

- gestion propre des coups legaux selon `lastMove`
- minimax + alpha-beta
- approfondissement iteratif
- heuristique meta-grille + sous-grilles
- gestion des sous-plateaux termines (victoire ou nul local)
- `main.cpp` clarifie et parametre (mode/niveau/nb parties)

## Organisation des fichiers

- `main.cpp`: boucle de jeu + configuration d'execution
- `Plateau.h/.cpp`: logique UTTT, generation des coups, evaluation, minimax
- `main.h`: interface fournie par la lib du template

## Lancement (usage applicatif)

Le programme accepte des arguments:

```bash
./UTTT_Template [mode] [level] [nbGames] [alwaysFirst] [alias]
```

- `mode`: `arena` ou `debug` (defaut `arena`)
- `level`: `easy1`, `easy2`, `med1`, `med2`, `hard1`, `hard2`, `vhard1`, `vhard2` (defaut `med1`)
- `nbGames`: entier > 0 (defaut `100`)
- `alwaysFirst`: `0` ou `1` (defaut `0`)
- `alias`: nom affiche (defaut `AGA`)

Exemples:

```bash
./UTTT_Template arena med1 100 0 AGA
./UTTT_Template debug hard1 10 1 AGA
```

## Build Windows (VM `win-dev`)

Commande qui fonctionne sur la VM:

```bash
g++ -std=c++17 -O2 -fno-lto main.cpp Plateau.cpp -L. -lUTTTLib allegro_font-5.2.dll allegro_ttf-5.2.dll allegro_image-5.2.dll allegro_primitives-5.2.dll allegro-5.2.dll -o ia_local.exe
```

Note:
- sans `-fno-lto`, la compilation echoue (mismatch LTO avec `libUTTTLib.a`)

## Execution via SSH (VM)

Execution directe en SSH:
- ne marche pas avec Allegro (`Failed to create display!`)

Execution qui marche:
- lancer l'exe via une tache planifiee interactive (`schtasks /IT`)
- rediriger la sortie vers un fichier log

Exemple:

```bat
schtasks /Create /TN UTTT_Run /SC ONCE /ST 23:59 /TR "cmd /c cd /d C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA && ia_local.exe arena med1 1 0 AGA > C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA\run_arena.log 2>&1" /RU ayoub /IT /F
schtasks /Run /TN UTTT_Run
```

Puis lecture du log:
- `C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA\run_arena.log`

## Remarques environnement

Le template fourni est centre Windows/MinGW (`.dll`, `.exe`, `libUTTTLib.a` au format COFF).
Sur macOS, le linking natif peut echouer sans toolchain compatible.

## Ce qui va

- la branche `ayoub_medium2` compile sur la VM Windows cible
- l'IA joue bien (coups et evaluations visibles dans les logs)
- base de code assez claire pour la soutenance

## Resultats de tests (VM `win-dev`, 2026-04-30)

Campagnes lancees en mode `arena` via tache interactive, puis parsing des logs.

| Niveau | Variante | Parties detectees | Victoires (PLAYER) | Defaites (IA) | Egalites | Taux de victoire (hors egalites) | Validation 80% |
|---|---|---:|---:|---:|---:|---:|---|
| `MEDIUM_1` | base `ayoub` | 100 | 98 | 0 | 2 | 100.00% | OK |
| `MEDIUM_2` | base `ayoub` | 100 | 15 | 83 | 2 | 15.31% | KO |
| `MEDIUM_2` | step1 `m2 step1 tactique simple` | 100 | 21 | 76 | 3 | 21.65% | KO |
| `MEDIUM_2` | step2 `m2 step2 filtre tactique` | 100 | 17 | 81 | 1 | 17.35% | KO |
| `MEDIUM_2` | step3 `m2 step3 eval anyboard centre coins` | 100 | 13 | 83 | 4 | 13.54% | KO |
| `MEDIUM_2` | step3b `m2 step3 anyboard only` | 100 | 15 | 81 | 3 | 15.62% | KO |
| `MEDIUM_2` | step4 `m2 step4 eval continue meta anyboard` | 100 | 13 | 83 | 3 | 13.54% | KO |
| `MEDIUM_2` | step4a `m2 palier1 variante a anyboard doux` | 100 | 14 | 81 | 5 | 14.74% | KO |
| `MEDIUM_2` | step4b `m2 palier1 variante b defense meta` | 100 | 11 | 83 | 6 | 11.70% | KO |
| `MEDIUM_2` | step5 `m2 palier2 killer moves simple` | 100 | 12 | 84 | 3 | 12.50% | KO |
| `MEDIUM_2` | step6 `m2 palier3 table transposition simple` | 100 | 21 | 75 | 3 | 21.88% | KO |

Note: dans les logs, les egalites apparaissent comme `IA AND PLAYER`.
Conclusion provisoire: `step1` reste la meilleure variante testee a date (`21.65%`).
Le `step3b` (ANY_BOARD seul) n'apporte pas de gain net par rapport a `step1`, donc
pas d'integration de `Killer Moves` a ce stade.
Le `step4` (Palier 1 sans filtres racine) est aussi en regression.
Les variantes `step4a`, `step4b` et `step5` ont aussi regresse.
Le `step6` (table de transposition simple) apporte un petit gain net vs `step1`.

## Iteration en cours (Palier 1)

- `m2 step4 retire filtres racine`:
  - suppression des regles binaires a la racine dans `prochainMove`
  - objectif: decision pilotee par l'evaluation continue
- `m2 step4 eval continue meta anyboard` (a benchmarker):
  - poids explicites centre > coins > bords sur la meta-grille
  - renforcement des menaces meta a 2-en-ligne
  - bonus/malus `ANY_BOARD` adouci

## Ce qui ne va pas encore

- execution SSH directe impossible (GUI Allegro requise)
- mode Arena semble enchaîner beaucoup de parties meme avec `nbGames` faible
- `MEDIUM_2` est loin de l'objectif de 80%
- `step2` n'a pas ameliore le score par rapport a `step1`

## Reste a faire

- verifier stabilite en mode `DEBUG` et `ARENA`
- repartir de `step1` et tester un `step3` simple
- ajouter un `.gitignore` pour `bin/`, `obj/`, executables et fichiers temporaires

## Sources -> Partie du code utilisee

Objectif de cette section: tracer precisement quelles sources ont influence quelles
modifications, pour faciliter la soutenance et eviter les changements "boite noire".

### Sources projet / protocole

- Sujet officiel ESILV (`Projet IA Ultimate Tic Tac Toe.pdf`)
  - utilise pour:
    - regles de redirection vers la sous-grille suivante
    - logique `ANY_BOARD` quand la sous-grille cible est terminee
    - metrique de validation: 80% en mode Arena, egalites exclues
- `HANDOFF-TICTACTOE.md` + `vm-notes-ayoub.md`
  - utilise pour:
    - protocole d'execution VM (tache interactive `schtasks /IT`)
    - build MinGW avec `-fno-lto`
    - methode de parsing des logs (`wins/losses/draws/starts`)

### Sources algorithmiques (deepsearch)

- `Optimisation Bot UTTT _ Feuille de Route.md`
  - Palier 1 (evaluation): inspire la ponderation territoriale
    centre > coins > bords sur la meta-grille/sous-grilles
  - Palier 1 (mobilite): inspire le bonus/malus "coup libre" (`ANY_BOARD`)
  - recommandation de methode:
    - supprimer les biais de racine qui perturbent Minimax
    - prioriser des ajustements d'evaluation avant d'ajouter de la complexite

### Liens externes retenus (via deepsearch) et mapping code

- AIMA - Adversarial Search:
  [https://aima.cs.berkeley.edu/4th-ed/pdfs/newchap06.pdf](https://aima.cs.berkeley.edu/4th-ed/pdfs/newchap06.pdf)
  - utilise comme base conceptuelle Minimax/Alpha-Beta
- ChessProgramming - Move Ordering:
  [https://www.chessprogramming.org/Move_Ordering](https://www.chessprogramming.org/Move_Ordering)
  - utilise comme reference pour les futures evolutions (Killer Moves / History)
- ChessProgramming - Killer Heuristic:
  [https://www.chessprogramming.org/Killer_Heuristic](https://www.chessprogramming.org/Killer_Heuristic)
  - candidate pour une prochaine iteration uniquement si gain net confirme
- ChessProgramming - Zobrist Hashing:
  [https://www.chessprogramming.org/Zobrist_Hashing](https://www.chessprogramming.org/Zobrist_Hashing)
  - reference cible pour une future table de transposition (non integree ici)

### Mapping concret sur cette branche

- `m2 step1 tactique simple`:
  - garde-fou tactique racine: victoire meta immediate + filtrage des reponses
    adverses gagnantes immediates
- `m2 step2 filtre tactique`:
  - tri tactique racine additionnel (regression constatee)
- `m2 step3 eval anyboard centre coins`:
  - desactivation effective du tri racine `step2`
  - ajout du bonus/malus de mobilite `ANY_BOARD` dans l'evaluation de feuille
  - renforcement de la ponderation centre/coins dans l'evaluation statique
