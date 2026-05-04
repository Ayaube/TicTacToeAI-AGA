# UTTT - Notes d'equipe (version chill)

## C'est quoi ce repo ?
En gros c'est notre bot pour Ultimate Tic-Tac-Toe en C++.
Le but c'est de passer les niveaux du sujet sans faire un monstre imbitable.

## La version qu'on garde pour la soutenance
On est partis sur une base simple:
- moteur clair
- algo explicable
- pas de magie noire

Concretement:
- minimax
- alpha-beta
- tri de coups
- evaluation meta-grille + sous-grilles
- petit garde-fou tactique au debut du tour

## Resultat actuel qui compte
Campagne MEDIUM_1 sur 100 parties terminees:
- 100 victoires
- 0 defaite
- 0 nul
- donc 100% hors egalites

Log garde dans le repo:
- `test-logs/run_med1_soutenance_100done.log`

## Ce qu'on a change vs template du prof
Le template du prof c'etait surtout un squelette qui envoie des coups.
Nous on a ajoute:
- la vraie gestion du plateau UTTT
- les coups legaux selon la redirection
- l'eval du plateau
- la recherche minimax/alpha-beta

Fichiers importants:
- `main.cpp`: boucle de jeu + config de run
- `Plateau.h/.cpp`: toute la logique IA
- `main.h`: interface fournie

## Build (VM windows)
Commande qui marche:

```bat
g++ -std=c++17 -O2 -fno-lto main.cpp Plateau.cpp -L. -lUTTTLib allegro_font-5.2.dll allegro_ttf-5.2.dll allegro_image-5.2.dll allegro_primitives-5.2.dll allegro-5.2.dll -o ia_soutenance.exe
```

## Run rapide
On lance en arena med1:

```bat
ia_soutenance.exe arena med1 100 0 AGA
```

## Pourquoi on a fait simple
Parce que c'est plus safe a expliquer a l'oral.
On prefere un algo propre qu'on comprend a 100% plutot qu'un truc ultra avance mais impossible a defendre.

## Suite prevue
Prochaine etape:
- tester une version MCTS propre
- garder minimax en fallback
- comparer les deux sur des campagnes longues (pas juste 3 parties vite fait)
