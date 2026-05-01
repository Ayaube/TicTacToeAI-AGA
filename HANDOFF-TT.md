# Handoff - Medium 2 / Palier 3 TT

Date: 2026-05-01

## Etat actuel

- Branche a reprendre: `ayoub_medium2_tt`
- Remote: `origin/ayoub_medium2_tt`
- Dernier commit utile: `b955909` (`m2 palier3 retour profondeur 9`)
- Base stable avant TT: `step1` (`95b018b`) avec `21.65%` contre `MEDIUM_2`
- Meilleure version actuelle: `step6b`, table de transposition + meilleur coup priorise, `23.16%`
- Objectif final: valider `MEDIUM_2` a 80% en mode Arena (egalites exclues)

## Resultats importants

| Variante | Idee | Resultat MEDIUM_2 |
|---|---|---:|
| baseline `ayoub` | base avant medium2 | 15.31% |
| `step1` | tactique racine simple | 21.65% |
| `step2` | filtre/tri tactique racine | 17.35% |
| `step3` | ANY_BOARD + centre/coins | 13.54% |
| `step3b` | ANY_BOARD seul | 15.62% |
| `step4` | evaluation continue sans filtres racine | 13.54% |
| `step4a` | anyboard doux | 14.74% |
| `step4b` | defense meta | 11.70% |
| `step5` | killer moves simple | 12.50% |
| `step6` | TT simple | 21.88% |
| `step6b` | TT + meilleur coup priorise | 23.16% |
| `step6c` | TT + profondeur 11 | 22.11% |

Conclusion: les reglages d'evaluation et les heuristiques racine ont souvent degrade. Le seul axe qui a donne un gain net est la table de transposition.

## Ce qui est implemente dans `step6b`

- `Plateau.cpp`
  - `TTEntry` avec:
    - `key`
    - `depth`
    - `score`
    - `flag`
    - `bestMove`
    - `generation`
  - table globale `g_transpositionTable` de taille `1 << 20`
  - hash d'etat par recomputation:
    - grille 9x9
    - meta-grille 3x3
    - `lastMove`
    - joueur courant
  - lecture TT au debut de `minimax`
  - stockage TT apres recherche du noeud
  - meilleur coup TT ajoute au tri avec un gros bonus
  - generation incrementee a chaque `prochainMove`
- `Plateau.h`
  - `PROFONDEUR_MAX = 9`

## Points a comprendre avant de changer

- Le code actuel a ete remis sur la logique `step1` + TT. Ne pas repartir des variantes Palier 1 qui ont regresse.
- `step1` contient encore des garde-fous racine:
  - jouer une victoire meta immediate
  - filtrer les coups qui donnent une victoire meta immediate a l'adversaire
  Ces garde-fous ont ete utiles statistiquement jusqu'ici.
- La TT actuelle est volontairement simple. Elle n'est pas un Zobrist incremental.
- La generation TT est incrementee a chaque coup joueur (`prochainMove`), donc la table est surtout utile pendant l'approfondissement iteratif d'un coup donne.

## Pistes serieuses pour la suite

### 1. Conserver la TT entre coups

Idee: ne pas invalider toute la table a chaque `prochainMove`. Le hash contient deja le plateau complet, `lastMove` et le joueur courant, donc certaines entrees d'une recherche precedente peuvent rester valides.

Pourquoi c'est prometteur:
- pendant notre coup N, la recherche explore souvent les reponses adverses possibles
- au coup N+1, l'etat reel peut etre un etat deja explore
- cela donne plus de valeur a l'approfondissement iteratif et a la TT

Risque:
- collisions plus visibles si la table vit plus longtemps
- il faut garder une politique de remplacement simple et robuste

Implementation minimale:
- supprimer ou adoucir `g_ttGeneration++` dans `prochainMove`
- garder `generation` seulement pour pouvoir reset au debut d'une partie si necessaire
- benchmark 100 parties

### 2. Ajouter un vrai Zobrist incremental

Idee: remplacer le hash recalcule en O(81) par un hash incrementable dans `jouerCoup` / `annulerCoup`.

Pourquoi c'est prometteur:
- la TT gagne si le hash est tres bon marche
- actuellement chaque noeud paie un scan complet du plateau

Risque:
- plus de surface de bug: `annulerCoup` doit restaurer exactement le hash
- il faut inclure meta-grille et contrainte `lastMove`

Approche prudente:
- garder le hash actuel comme reference debug temporaire
- ajouter un `m_hashPieces` ou recalculer seulement pieces/meta avec Zobrist
- tester quelques positions avec comparaison hash incremental vs recalcul complet

### 3. Best move TT a la racine

Idee: appliquer explicitement le meilleur coup TT au tri des coups dans `prochainMove`, pas seulement dans les noeuds internes de `minimax`.

Pourquoi c'est raisonnable:
- l'approfondissement iteratif remonte deja le meilleur coup en tete
- mais le meilleur coup TT pourrait aider les coups racine au debut d'une nouvelle profondeur

Risque:
- gain probablement faible

### 4. Historique de coups tres leger

Idee: remplacer le `Killer Moves` qui a regresse par une history heuristic simple indexee par case globale `row * 9 + col`.

Pourquoi:
- `Killer Moves` par profondeur et coordonnee seule est peut-etre trop peu contextualise en UTTT
- une history globale peut mieux capter les cases qui coupent souvent

Risque:
- peut biaiser le tri et refaire les regressions observees
- a tester seulement apres avoir pousse plus loin la TT

### 5. Ne pas prioriser maintenant

- Quiescence search: risque d'explosion combinatoire, pas prioritaire.
- Nouveaux poids d'evaluation: deja plusieurs regressions, a eviter tant qu'on n'a pas plus de signal.
- MCTS/RL/opening book: hors scope et trop couteux pour le projet actuel.

## Commandes VM utiles

Build:

```bat
g++ -std=c++17 -O2 -fno-lto main.cpp Plateau.cpp -L. -lUTTTLib allegro_font-5.2.dll allegro_ttf-5.2.dll allegro_image-5.2.dll allegro_primitives-5.2.dll allegro-5.2.dll -o ia_local.exe
```

Execution directe SSH:
- ne marche pas (`Failed to create display!`)

Execution Arena:
- utiliser `schtasks /IT`
- surveiller `Start game #`
- arreter a 100 parties avec `taskkill /IM ia_local.exe /F`

Parsing local:

```bash
starts=$(rg -c "Start game #" test-logs/run_med2_x.log)
wins=$(rg -c "Game: Winner .*\\(PLAYER\\)" test-logs/run_med2_x.log)
losses=$(rg -c "Game: Winner .*\\(IA\\)" test-logs/run_med2_x.log)
draws=$(rg -c "IA AND PLAYER|NO_WINNER" test-logs/run_med2_x.log)
```

## Recommendation au successeur

Continuer sur `ayoub_medium2_tt`, pas sur `ayoub_medium2`. Le meilleur point de depart est l'etat courant avec TT + meilleur coup et profondeur 9.

Premier test conseille:
- faire une branche courte depuis `ayoub_medium2_tt`
- conserver la table entre coups
- benchmark `MEDIUM_2` 100 parties
- garder seulement si score strictement > 23.16%

Seuil mental:
- moins de 23%: rollback
- 23-25%: petit gain, continuer TT
- au-dessus de 30%: changement significatif, documenter et faire un second run pour confirmer
