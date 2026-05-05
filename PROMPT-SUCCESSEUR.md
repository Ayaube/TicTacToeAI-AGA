# Prompt pour le successeur Codex

Tu reprends le depot `TicTacToeAI-AGA` apres une serie d'essais minimax sur Ultimate Tic-Tac-Toe.

## Etat actuel

- Branche courante attendue: `ayoub_fusion_opti`.
- Base historique stable: `ayoub_soutenance`.
- Version stable a ne pas casser: minimax alpha-beta sur `ayoub_soutenance`, validee a 100% contre `MEDIUM_1`.
- Les pistes MCTS ont ete abandonnees.
- L'utilisateur va te fournir:
  - un nouveau DeepSearch;
  - un resultat d'analyse des logs par Claude Opus;
  - un peu de reverse engineering de la librairie / des logs.

Lis ces nouveaux elements avant de coder. Ne repars pas sur PVS, MCTS ou TT par reflexe.

## Branche fusion actuelle

La branche `ayoub_fusion_opti` contient un essai non valide:

- heuristique coups libres:
  - `scorerCoupRapide`: un coup qui envoie l'adversaire vers une sous-grille terminee vaut `-500` au lieu de `+100`;
  - `evaluer(GameMove last, int joueurCourant)`: ajoute `joueurCourant * 800` si le dernier coup donne un coup libre au joueur courant.
- PVS + History Heuristic:
  - `TIMEOUT_MS = 1000`;
  - marge interne 50 ms, coupure effective vers 950 ms;
  - `m_history[81]`;
  - PVS / null-window dans `minimax`;
  - bonus history `depth * depth` sur coupure alpha-beta.

## Resultat de bench fusion

Log local:

```text
test-logs/run_fusion_med2.log
```

Resultat `MEDIUM_2` reel sur 100 parties:

- 21 victoires;
- 76 defaites;
- 3 nuls;
- winrate hors nuls: `21 / 97 = 21.65%`.

Conclusion: l'essai fusion ne bat pas la meilleure reference connue `step6b TT meilleur coup` a 23.16%, et ne casse pas le seuil des 25%.

## References utiles

- `test-logs/run_fusion_med2.log`: log complet de l'essai fusion.
- `test-logs/run_med1_soutenance_100done.log`: preuve stable `MEDIUM_1`.
- Handoff local hors repo: `/Users/aubepine/Documents/Coding/Projet TicTacToe/HANDOFF-TICTACTOE.md`.
- Notes VM hors repo: `/Users/aubepine/Documents/Coding/Projet TicTacToe/vm-notes-ayoub.md`.

## Regles importantes

- Ne modifie pas `main.h` ni la librairie fournie.
- Ne lance pas un test `MEDIUM_2` sans verifier dans le log: `Game: - Level           : Medium 2`.
- Pour la VM, utiliser une tache planifiee interactive; le lancement direct SSH echoue souvent avec Allegro.
- Apres chaque bench VM, verifier `tasklist` et stopper le process si l'arene reste ouverte.
- Ne pas ajouter `Guide-Soutenance-Technique.md` ou `Soutenance-IA.md` sans decision explicite: ils etaient non suivis.

## Prochaine mission probable

1. Lire le nouveau DeepSearch et l'analyse Claude Opus fournis par l'utilisateur.
2. Extraire une hypothese precise a tester.
3. Faire un patch tres local.
4. Compiler sans warnings.
5. Tester sur `MEDIUM_2` reel avec un log complet.
6. Comparer au seuil historique:
   - reference minimax/TT: 23.16%;
   - objectif ambitieux: 25%+.
