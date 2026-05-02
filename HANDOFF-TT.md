# Handoff - Medium 2 / Palier 3 TT

Date: 2026-05-02

## Etat actuel

- Branche a reprendre: `ayoub_medium2_tt`
- Remote: `origin/ayoub_medium2_tt`
- Dernier commit utile pour le code: `b955909` (`m2 palier3 retour profondeur 9`)
- Base stable avant TT: `step1` (`95b018b`) avec `21.65%` contre `MEDIUM_2`
- Meilleure version actuelle: `step6b`, table de transposition + meilleur coup priorise, `23.16%`
- Derniere piste testee: `step7` TT persistante entre coups, regression a `20.21%`, rollback applique
- Autre piste testee: `step8` hash incremental, regression a `22.22%`, rollback applique
- Autre piste testee: `step9` remplacement TT profondeur d'abord, regression a `15.22%`, rollback applique
- Autre piste testee: `step10` tri racine avec table, regression a `18.95%`, rollback applique
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
| `step7` | TT conservee entre coups | 20.21% |
| `step8` | hash incremental | 22.22% |
| `step9` | remplacement TT profondeur d'abord | 15.22% |
| `step10` | tri racine par scores TT | 18.95% |

Conclusion: les reglages d'evaluation et les heuristiques racine ont souvent degrade. Le seul axe qui a donne un gain net est la table de transposition, mais la conservation simple entre coups, le hash incremental simple, le remplacement profondeur d'abord et le tri racine par scores TT ont regresse.

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
- Cette invalidation par coup doit rester en place pour l'instant: le test `step7` qui gardait la TT pendant toute la partie a fait `19/75/6`, soit `20.21%`.
- Le hash incremental simple (`step8`) gardait la meme formule de cle mais maintenait grille/meta en O(1); il a fait `20/70/10`, soit `22.22%`, donc il a aussi ete revert.
- Le remplacement TT profondeur d'abord (`step9`) a fait `14/78/8`, soit `15.22%`, donc garder les entrees profondes au lieu de remplacer simplement n'aide pas ici.
- Le tri racine par scores TT (`step10`) a fait `18/77/5`, soit `18.95%`; le simple swap du meilleur coup apres chaque iteration reste meilleur.
- Instrumentation TT disponible avec `-DTT_STATS`; premier run court: hit rate `27.31%`, usable/hits `3.91%`, collision rate `0.25%`, first-move cutoff `74.55%`.

## Pistes serieuses pour la suite

### 1. Ne pas conserver simplement la TT entre coups

Idee testee le 2026-05-02: ne pas invalider toute la table a chaque `prochainMove`, mais seulement a la creation d'un nouveau `Plateau`.

Pourquoi c'etait prometteur:
- pendant notre coup N, la recherche explore souvent les reponses adverses possibles
- au coup N+1, l'etat reel peut etre un etat deja explore
- cela donne plus de valeur a l'approfondissement iteratif et a la TT

Resultat:
- commit teste: `052513c` (`m2 garde la table entre coups`)
- benchmark: `run_med2_step7_ttpersist.log`
- stats: 100 starts, 19 wins, 75 losses, 6 draws
- score hors egalites: `20.21%`
- decision: rollback, ne pas reprendre cette variante simple sans changer la politique de remplacement/hash.

### 2. Hash incremental simple teste puis revert

Idee testee le 2026-05-02: remplacer le hash recalcule en O(81) par un hash maintenu dans `setCase`, `verifPlateau`, `jouerCoup` et `annulerCoup`, sans changer la formule de cle.

Resultat:
- commit teste: `29a617e` (`m2 ajoute hash incremental`)
- benchmark: `run_med2_step8_hashinc.log`
- stats: 100 starts, 20 wins, 70 losses, 10 draws
- score hors egalites: `22.22%`
- decision: rollback, ne pas reprendre ce hash incremental simple sans autre changement.

Interpretation possible:
- le hash etait valide localement contre une recomputation complete
- le changement de cout/timing peut modifier les profondeurs atteintes et la distribution des decisions
- le gain brut de CPU ne suffit pas a battre `step6b` dans ce protocole.

### 3. Remplacement TT profondeur d'abord teste puis revert

Idee testee le 2026-05-02: garder la TT par coup, mais ne remplacer une entree existante de la generation courante que si la nouvelle recherche est au moins aussi profonde.

Resultat:
- commit teste: `665f856` (`m2 remplace tt par profondeur`)
- benchmark: `run_med2_step9_ttdepth.log`
- stats: 100 starts, 14 wins, 78 losses, 8 draws
- score hors egalites: `15.22%`
- decision: rollback, conserver le remplacement simple de `step6b`.

Interpretation possible:
- dans cette table directe de taille fixe, refuser des remplacements peut garder des entrees profondes mais moins pertinentes pour les branches courantes
- le remplacement simple semble mieux s'accorder avec l'approfondissement iteratif actuel.

### 4. Pistes TT restantes prudentes

Pistes possibles, a tester une par une:
- exploiter l'instrumentation TT deja ajoutee avec `-DTT_STATS`
- cibler le faible taux de hits utilisables (`3.91%` des hits sur le premier run), pas la taille de table
- tester un bonus TT interne plus modere que `100000`, seulement si les compteurs montrent une sur-priorisation
- eviter les changements aveugles de tri racine: `step10` a regresse.

### Stats TT de reference

Run: `run_med2_ttstats_20.log`, `MEDIUM_2`, Arena court instrumente.

Synthese:
- lignes `[TT]`: 515
- probes: 3,189,652
- hits: 871,198 (`27.31%`)
- usable: 34,099 (`3.91%` des hits, `1.07%` des probes)
- exact/lower/upper returns: 937 / 28,959 / 4,203
- stores: 3,145,574
- collisions: 7,972 (`0.25%`)
- TT bestMove legal/known: `96.09%`
- TT bestMove first/legal: `99.97%`
- first-move cutoff rate: `74.55%`
- avg hit depth: `1.51`
- avg completed depth: `6.48`

Lecture:
- collisions faibles: agrandir la TT n'est probablement pas le premier levier
- meilleur coup TT deja tres dominant dans le tri interne
- tres peu de hits retournent un score directement; la TT actuelle agit surtout comme heuristique d'ordonnancement.

### 5. Best move TT a la racine

Idee testee partiellement par `step10`: utiliser la TT a la racine pour trier les coups avec les scores enfants de l'iteration precedente.

Pourquoi c'est raisonnable:
- l'approfondissement iteratif remonte deja le meilleur coup en tete
- mais le meilleur coup TT pourrait aider les coups racine au debut d'une nouvelle profondeur

Resultat:
- regression a `18.95%`
- ne pas reprendre cette forme simple.

### 6. Historique de coups tres leger

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
