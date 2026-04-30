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

Points actuellement en place:

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

## Lancement

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

## Remarque environnement

Le template fourni est centre Windows/MinGW (`.dll`, `.exe`, `libUTTTLib.a` au format COFF).
Sur macOS, le linking natif peut echouer sans toolchain compatible.

## Suite du travail

- consolider les tests Arena avec stats par niveau
- verifier proprement `MEDIUM_1` puis `MEDIUM_2`
- optimiser de facon progressive sans complexifier inutilement le code
- nettoyer les artefacts de build versionnes (`bin/`, `obj/`) via `.gitignore`
