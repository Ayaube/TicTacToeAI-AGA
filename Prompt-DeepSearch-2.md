# Prompt DeepSearch 2 - Chercher un autre algorithme que MCTS

Tu es un assistant de recherche algorithmique et ingénierie C++.

## Contexte projet

- Projet étudiant C++17: IA Ultimate Tic-Tac-Toe.
- Architecture volontairement simple:
  - `main.cpp`
  - `Plateau.h`
  - `Plateau.cpp`
- Objectif soutenance:
  - IA performante mais explicable par des étudiants.
  - code lisible, pas de dépendances lourdes.
  - pas de refactor massif.
  - garder une version stable qui bat `MEDIUM_1`.
- Branche stable:
  - `ayoub_soutenance`
  - validée contre `MEDIUM_1`.
- Branche expérimentale:
  - `ayoub_tentative_mcts`.

## Code actuel

- Représentation:
  - `std::array<std::array<int,9>,9> m_g`
  - `std::array<std::array<int,3>,3> m_e`
- Joueur bot = `1`.
- IA adverse = `-1`.
- Sous-grille nulle/terminée = `2`.
- Fonctions existantes utiles:
  - `getCoupsLegauxFast(last, buf)`
  - `jouerCoup(row, col, joueur)`
  - `annulerCoup(row, col, ancienEtat)`
  - `gagnantMetaGrille()`
  - `resultatPartie()`
  - `evaluer()`
  - `minimax(...)`
  - `prochainMoveMinimax(...)`
  - `prochainMoveMCTS(...)`

## Compilation / niveaux

Attention: le niveau est choisi à la compilation, pas par les arguments CLI.

- Par défaut: `MEDIUM_1`.
- `-DNIVEAU_MEDIUM_2`: compile en `MEDIUM_2`.
- `-DNIVEAU_HARD_1`: compile en `HARD_1`.
- `-DACTIVER_MCTS`: active le MCTS expérimental.

Toujours vérifier dans le log:

```text
Game: - Level : ...
```

## Version stable à préserver

- Version minimax alpha-beta avec heuristique.
- Approfondissement itératif.
- Tri de coups.
- Timeout interne autour de 350 ms.
- Résultat stable soutenance:
  - `MEDIUM_1`
  - 100 parties
  - 100 victoires
  - 0 défaite
  - 0 nul

Cette version ne doit pas être cassée.

## Résultats MCTS déjà testés

### 1. MCTS minimal

Implémentation:

- UCB1.
- Expansion classique.
- Playouts aléatoires jusqu’à fin de partie.
- Choix final par enfant le plus visité.
- Plusieurs dizaines de milliers de simulations par coup.
- Fallback minimax conservé par défaut.

Résultats:

- `MEDIUM_1`:
  - 100 parties
  - 98 victoires
  - 0 défaite
  - 2 nuls
  - conclusion: OK contre MEDIUM_1.
- `MEDIUM_2` réel:
  - build `-DACTIVER_MCTS -DNIVEAU_MEDIUM_2`
  - arrêt à 20 défaites
  - 28 parties démarrées
  - 4 victoires
  - 20 défaites
  - 3 nuls
  - conclusion: ne valide pas MEDIUM_2.
- `HARD_1` réel:
  - build `-DACTIVER_MCTS -DNIVEAU_HARD_1`
  - 100 parties
  - 1 victoire
  - 93 défaites
  - 6 nuls
  - conclusion: échec net.

### 2. Heavy playout tactique naïf

Règle testée:

- si un coup gagne une sous-grille ou la partie, le jouer;
- sinon si un coup bloque une victoire locale adverse, le jouer;
- sinon jouer aléatoirement.

Résultat `MEDIUM_2` réel:

- arrêt à 20 défaites.
- 22 parties démarrées.
- 0 victoire.
- 21 défaites.
- 0 nul.

Conclusion:

- régression nette.
- patch rollback.
- hypothèse: gagner/bloquer localement sans regarder la sous-grille envoyée donne souvent de mauvais coups en Ultimate Tic-Tac-Toe.

### 3. MCTS-EH recommandé par une précédente recherche

Règle testée:

- supprimer le playout aléatoire;
- évaluer directement le plateau étendu avec `evaluer()`;
- normaliser avec une sigmoïde;
- température testée: `8000.0`.

Résultat `MEDIUM_2` réel:

- kill switch: stop si 8 défaites dans les 15 premières parties.
- 9 parties démarrées.
- 0 victoire.
- 8 défaites.
- 0 nul.

Conclusion:

- régression immédiate.
- patch rollback.

## Anciennes tentatives minimax / table de transposition

Avant MCTS, plusieurs variantes minimax ont été testées contre `MEDIUM_2`.

Meilleure variante connue:

- Table de transposition simple + meilleur coup TT priorisé.
- Log nommé `step6b TT meilleur coup`.
- 100 parties:
  - 22 victoires
  - 73 défaites
  - 5 nuls
  - winrate hors égalités: 23.16%

Autres variantes TT testées et régressions:

- TT persistante entre coups: régression.
- hash incrémental simple: régression.
- remplacement TT profondeur d’abord: forte régression.
- tri racine par scores TT: régression.
- killer moves simple: régression.

## Problème actuel

On manque de temps.

Le MCTS a été testé sérieusement et n’est pas suffisant.

Les variantes rapides MCTS ont régressé.

On cherche maintenant **un autre algorithme ou une autre stratégie principale**, pas une nouvelle variante MCTS pure.

## Mission DeepSearch

1. Analyse les résultats ci-dessus.
2. Explique pourquoi MCTS et ses variantes rapides ne conviennent pas dans notre contexte.
3. Propose **un autre algorithme prioritaire** compatible avec le code actuel.
4. Propose **une stratégie de secours** si l’algorithme prioritaire échoue vite.
5. Le plan doit être réaliste pour une équipe étudiante avec peu de temps.
6. Le code final doit rester explicable en soutenance.

## Algorithmes à comparer

Compare maximum 4 approches, par exemple:

- minimax alpha-beta amélioré mais ciblé;
- search tactique à profondeur courte avant minimax;
- beam search / best-first search;
- proof-number search local;
- threat-space search adapté UTTT;
- expectimax ou autre stratégie probabiliste simple;
- approche hybride basée sur le minimax existant mais pas MCTS.

Tu peux proposer autre chose, mais évite les solutions lourdes.

## Contraintes très fortes

- C++17.
- Pas de dépendance externe.
- Pas de réseau de neurones.
- Pas de bitboards maintenant, sauf si tu expliques précisément pourquoi c’est indispensable et faisable vite.
- Pas de refactor massif.
- Pas de “réglage magique” avec 20 poids incompréhensibles.
- Ne pas casser la version stable `MEDIUM_1`.
- Garder un fallback simple.

## Livrables attendus

1. Résumé exécutif en 10 lignes maximum.
2. Diagnostic clair: pourquoi les essais MCTS ont échoué.
3. Comparatif de 3 ou 4 algorithmes maximum.
4. Choix final recommandé.
5. Plan d’implémentation incrémental:
   - étape 1;
   - objectif mesurable;
   - risque;
   - rollback.
6. Pseudo-code précis compatible avec notre API actuelle.
7. Fonctions à ajouter/modifier dans `Plateau.cpp` / `Plateau.h`.
8. Protocole de test:
   - test rapide;
   - kill switch;
   - campagne longue;
   - seuil pour garder ou rollback.
9. Estimation des lignes modifiées.
10. Liste claire de ce qu’il ne faut pas faire maintenant.

## Style attendu

- Français simple.
- Réponse concrète.
- Pas de blabla théorique.
- Pas de promesse non mesurable.
- Si tu recommandes une heuristique, explique-la en une phrase intuitive.
- Priorité au gain rapide mesurable.
- Il faut pouvoir défendre le choix devant un jury.
