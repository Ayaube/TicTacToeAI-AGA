# Ultimate Tic-Tac-Toe - Dossier Soutenance (version simple)

## Objectif du projet
Construire une IA C++ capable de jouer a Ultimate Tic-Tac-Toe contre les niveaux fournis par le sujet.

Critere de validation d'un niveau:
- au moins 80% de victoires en mode Arena
- egalites exclues du calcul

## Version retenue pour la soutenance
Cette version est volontairement simple a expliquer.

- Branche cible: `main`
- Base technique: logique `ayoub`
- Point cle: code assez court, lisible, sans heuristiques avancees difficiles a justifier a l'oral

## Resultat de reference (MEDIUM_1)
Campagne de validation executee sur VM Windows cible (meme environnement que le rendu), date du 3 mai 2026.

- starts: 100
- wins: 100
- losses: 0
- draws: 0
- taux hors egalites: 100.00%

Log de reference:
- local: `test-logs/run_med1_soutenance_100done.log`
- VM: `C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA\run_med1_soutenance_100done.log`

## Ce qui a change par rapport au template du prof
Le template du prof fournit surtout l'interface de jeu (`IGame`, `getMove`, `setMove`) et le squelette.

Les ajouts majeurs de notre cote sont:

1. Representation complete du plateau UTTT
- grille 9x9 des cases
- meta-grille 3x3 des sous-plateaux
- prise en compte des sous-plateaux gagnes ET des sous-plateaux nuls

2. Generation correcte des coups legaux
- respect de la redirection par `lastMove`
- bascule en mode `ANY_BOARD` quand la sous-grille cible est terminee

3. IA de recherche classique et defendable
- minimax
- alpha-beta
- approfondissement iteratif borne par temps
- tri simple des coups pour ameliorer les coupures

4. Evaluation heuristique lisible
- poids fort sur la meta-grille
- menaces a 2 alignes
- bonus positionnels centre/coins
- score local des sous-grilles non terminees

5. Petite couche tactique en racine (simple a expliquer)
- jouer un coup qui gagne la meta-grille immediatement si disponible
- eviter les coups qui donnent une victoire meta immediate a l'adversaire

## Pipeline d'un coup IA (explication orale)
A chaque tour:

1. Lire le dernier coup adverse
2. Recalculer les coups legaux
3. Si victoire meta immediate possible: la jouer
4. Sinon retirer les coups qui donnent une victoire meta immediate adverse
5. Lancer iterative deepening (minimax + alpha-beta) jusqu'a la limite de temps
6. Envoyer le meilleur coup trouve

## Pourquoi cette version est la plus pratique a soutenir
- pas de mecanismes avances (pas de Zobrist, pas de TT, pas de PVS/LMR)
- comportement stable et reproductible
- architecture claire: `main.cpp` pilote, `Plateau.cpp` contient les regles + IA
- resultat MEDIUM_1 tres au-dessus du seuil de 80%

## Structure des fichiers
- `main.cpp`: boucle de jeu et config de demo (MEDIUM_1)
- `Plateau.h/.cpp`: regles UTTT, coups legaux, evaluation, minimax
- `main.h`: interface du moteur fourni
- `test-logs/`: traces de campagnes

## Execution (version soutenance)
Cette version lance directement une campagne `MEDIUM_1` en Arena.

Initialisation utilisee:
- nbGames: 100
- level: MEDIUM_1
- mode: ARENA
- alwaysPlayFirst: false
- alias: AGA

## Build VM Windows (reference)
Commande utilisee sur la VM:

```bat
g++ -std=c++17 -O2 -fno-lto main.cpp Plateau.cpp -L. -lUTTTLib allegro_font-5.2.dll allegro_ttf-5.2.dll allegro_image-5.2.dll allegro_primitives-5.2.dll allegro-5.2.dll -o ia_soutenance.exe
```

Note:
- sans `-fno-lto`, echec de link avec `libUTTTLib.a`

## Run VM via tache interactive (SSH)
Execution directe en SSH ne cree pas le display Allegro.
La methode fiable est la tache interactive:

```bat
schtasks /Create /TN UTTT_Run /SC ONCE /ST 23:59 /TR "cmd /c cd /d C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA && ia_soutenance.exe arena med1 100 0 AGA > C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA\run_med1_soutenance_100done.log 2>&1" /RU ayoub /IT /F
schtasks /Run /TN UTTT_Run
```

## Elements a dire en soutenance (sans ouvrir le code)
- On est parti du template du prof (interface/moteur), puis on a implemente la logique UTTT complete.
- La force principale vient d'un minimax alpha-beta propre + evaluation meta-grille.
- On a volontairement garde une version simple et explicable.
- Cette version valide largement MEDIUM_1 (100% hors egalites sur 100 parties).
