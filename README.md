# Ultimate Tic-Tac-Toe — Feuille de route IA

## État actuel du code

### Ce qui fonctionne
- Structure de base : plateau 9x9, sous-plateaux, états
- Détection de victoire sur un sous-plateau (`gagnant`)
- Logique de base dans `prochainMove` : gagner, bloquer, heuristique simple

### Problèmes critiques à corriger

- `estCondamne` ne vérifie pas si un sous-plateau est **plein sans gagnant** (match nul local). Elle retourne `true` uniquement si quelqu'un a gagné la grille, ce qui peut provoquer des coups illégaux.
- Dans `prochainMove`, la redirection ne tient pas compte des sous-plateaux pleins (pas seulement condamnés).
- Aucune stratégie sur la méta-grille : l'IA ne raisonne pas sur quels sous-plateaux sont importants à gagner.
- Aucun algorithme de recherche : indispensable pour passer les niveaux MEDIUM et au-delà.

---

## Feuille de route

### Étape 1 — Corriger les bugs de règles (priorité absolue)

Un bot qui joue des coups illégaux perd des parties inutilement. À faire avant tout le reste.

- Ajouter une méthode `estPlein(int sousI, int sousJ)` qui vérifie si les 9 cases d'un sous-plateau sont toutes occupées.
- Modifier `estCondamne` (ou créer une méthode `estJouable`) pour qu'elle retourne `false` si le sous-plateau est gagné **ou** plein.
- Vérifier que le fallback (jouer n'importe où quand on est envoyé sur une grille non jouable) respecte bien cette règle.

### Étape 2 — Refactoriser la représentation du plateau

Cette étape simplifie tout le code et est indispensable pour l'algorithme de recherche.

- Ajouter une méthode `getCoupsLegaux(GameMove& lastMove)` qui retourne un `vector<GameMove>` contenant tous les coups légaux selon le dernier coup joué.
- Ajouter une méthode `estTermine()` qui retourne `true` si la partie est finie (méta-grille gagnée ou plus aucun coup possible).
- Ajouter une méthode `getEtatMetaGrille()` propre pour consulter l'état des 9 sous-plateaux.

### Étape 3 — Implémenter Minimax avec élagage Alpha-Bêta

C'est l'étape clé pour passer les niveaux MEDIUM et HARD.

Le principe de Minimax : explorer récursivement tous les coups possibles jusqu'à une certaine profondeur, en alternant entre maximiser son score et minimiser celui de l'adversaire. L'élagage Alpha-Bêta permet de couper les branches inutiles et de doubler ou tripler la profondeur explorable pour le même temps de calcul.

- Implémenter `minimax(plateau, profondeur, alpha, beta, joueurMax)` qui retourne un score.
- Commencer avec une profondeur de 3, puis augmenter progressivement.
- Appeler Minimax depuis `prochainMove` pour choisir le meilleur coup.

### Étape 4 — Écrire une bonne fonction d'évaluation

C'est ce qui différencie une IA faible d'une IA forte. La fonction doit scorer une position sans aller jusqu'au bout de la partie.

Critères à prendre en compte, du plus au moins important :

- Victoire ou défaite sur la méta-grille (valeur absolue très élevée, ex: +10000 / -10000)
- Nombre de sous-plateaux gagnés par chaque joueur, avec un bonus pour le centre et les coins de la méta-grille
- Menaces à deux dans un sous-plateau (deux symboles alignés avec la troisième case libre)
- Contrôle du centre dans le sous-plateau courant
- Malus si on envoie l'adversaire dans un sous-plateau favorable pour lui

### Étape 5 — Optimisations avancées (pour HARD et VERY_HARD)

- **Zobrist hashing** : mémoriser les positions déjà évaluées dans une table de transposition pour éviter de recalculer la même position plusieurs fois.
- **Iterative deepening** : chercher à profondeur 1, puis 2, puis 3, etc., en s'arrêtant si le temps de calcul approche la limite du timeout. Cela garantit toujours d'avoir un coup valide prêt.
- **Tri des coups** : évaluer d'abord les coups qui semblent les meilleurs (centre, menaces) pour améliorer l'efficacité de l'élagage Alpha-Bêta.

---

## Résumé des priorités

| Priorité | Tâche | Niveau visé |
|----------|-------|-------------|
| Immédiat | Corriger `estCondamne` + gestion des plateaux pleins | Corriger les coups illégaux |
| Immédiat | Refactoriser `getCoupsLegaux()` | Base de tout le reste |
| Court terme | Implémenter Minimax profondeur 3 | Passer EASY |
| Court terme | Ajouter Alpha-Bêta | Passer MEDIUM |
| Moyen terme | Améliorer la fonction d'évaluation | Passer HARD |
| Long terme | Zobrist + iterative deepening | Tenter VERY_HARD |
