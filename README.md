# Ultimate Tic-Tac-Toe - Projet IA (ESILV A3)

## Objectif
Coder une IA en C++ (POO) capable d'affronter les IA du sujet sur plusieurs niveaux:

- `EASY_1`, `EASY_2`
- `MEDIUM_1`, `MEDIUM_2`
- `HARD_1`, `HARD_2`
- `VERY_HARD_1`, `VERY_HARD_2`

Validation d'un niveau: au moins 80% de victoires en mode Arena (hors egalites).

## Etat de la branche `ayoub`

Cette branche est le nouveau point de depart technique:

- base fusionnee depuis `Antoine`
- port manuel limite de correctifs utiles depuis `gabin`
- code garde volontairement simple et lisible

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

- la branche `ayoub` compile sur la VM Windows cible
- l'IA joue bien (coups et evaluations visibles dans les logs)
- base de code assez claire pour la soutenance

## Resultats de tests (VM `win-dev`, 2026-04-30)

Campagnes lancees en mode `arena` via tache interactive, puis parsing des logs.

| Niveau | Parties detectees | Victoires (PLAYER) | Defaites (IA) | Egalites | Taux de victoire (hors egalites) | Validation 80% |
|---|---:|---:|---:|---:|---:|---|
| `MEDIUM_1` | 100 | 98 | 0 | 2 | 100.00% | OK |
| `MEDIUM_2` | 100 | 15 | 83 | 2 | 15.31% | KO |

Note: dans les logs, les egalites apparaissent comme `IA AND PLAYER`.

## Ce qui ne va pas encore

- execution SSH directe impossible (GUI Allegro requise)
- mode Arena semble enchaîner beaucoup de parties meme avec `nbGames` faible
- `MEDIUM_2` est loin de l'objectif de 80%

## Reste a faire

- extraire des stats claires depuis les logs (wins/loss/draw + pourcentage hors egalites)
- verifier stabilite en mode `DEBUG` et `ARENA`
- ameliorer l'IA pour monter le taux en `MEDIUM_2` (evaluation + gestion tactique)
- ajouter un `.gitignore` pour `bin/`, `obj/`, executables et fichiers temporaires
