# Journal implementation MCTS

Date de depart: 2026-05-04
Branche: `ayoub_tentative_mcts`

## But

Ajouter une tentative MCTS simple et explicable sans casser la version minimax stable.
Le minimax doit rester disponible comme fallback a tout moment.

## Etat initial retenu

- Version stable: minimax alpha-beta avec approfondissement iteratif.
- Preuve baseline: `test-logs/run_med1_soutenance_100done.log`.
- Resultat baseline lu: 100 victoires joueur, 0 victoire IA, winrate joueur 100%.
- Le coeur IA est dans `Plateau.cpp`.
- Representation actuelle: tableaux fixes `std::array`, pas de bitboards.
- Choix de depart: ne pas refondre en bitboards tant que les mesures ne le justifient pas.

## Plan d'attaque

1. Preparer l'API interne sans changer le comportement minimax.
   - Objectif: exposer proprement les briques utiles aux simulations.
   - Mesure: compilation OK et baseline minimax toujours OK.
   - Rollback: revenir au `prochainMove` actuel.

2. Ajouter une fonction terminale complete pour les simulations.
   - Objectif: detecter victoire meta, nul, ou victoire au nombre de sous-grilles si le plateau est fini.
   - Mesure: aucun coup illegal, pas de divergence visible sur campagne courte.
   - Rollback: la garder inutilisee par minimax si doute.

3. Ajouter un MCTS minimal derriere un switch.
   - Objectif: MCTS joue legalement sous budget temps.
   - Mesure: logs `simulations`, `simulations/sec`, `temps_ms`.
   - Rollback: switch par defaut sur minimax.

4. Ajouter instrumentation.
   - Objectif: comprendre les performances avant optimisation.
   - Mesure: chaque campagne a un log exploitable.
   - Rollback: flag de logs desactive.

5. Ajouter un playout legerement guide si necessaire.
   - Objectif: corriger les erreurs tactiques simples.
   - Mesure: meilleur winrate que MCTS aleatoire pur.
   - Rollback: option playout aleatoire pur.

6. Comparer et decider.
   - Objectif: garder MCTS seulement s'il progresse sans sacrifier la stabilite.
   - Mesure: MEDIUM_1 >= 95% sur 100 parties avant toute suite.
   - Rollback: minimax reste version soutenance.

## Connexion VM / tests

- Aucun script VM trouve dans le repo au depart.
- Depuis macOS, les `.exe` Windows ne se lancent pas directement.
- `wine` absent.
- `x86_64-w64-mingw32-g++` absent.
- La compilation objet locale avec Apple clang sert de garde-fou C++ avant les tests VM.
- Notes VM utiles lues dans `../vm-notes-ayoub.md`.
- SSH OK vers `win-dev`.
- Repo principal VM detecte sale et sur ancienne branche `ayoub_medium2`; ne pas l'ecraser.
- Strategie test VM: creer un dossier de bench separe, copier les sources courantes, construire et lancer les campagnes dedans.
- Commande README pour Windows:
  `g++ -std=c++17 -O2 -fno-lto main.cpp Plateau.cpp -L. -lUTTTLib allegro_font-5.2.dll allegro_ttf-5.2.dll allegro_image-5.2.dll allegro_primitives-5.2.dll allegro-5.2.dll -o ia_soutenance.exe`
- Pour tester MCTS cote VM, ajouter `-DACTIVER_MCTS` a la commande de build.
- Run via SSH direct impossible a cause d'Allegro: utiliser `schtasks /IT`, puis lire le log et arreter a 100 parties avec `taskkill`.

## Resultats de tests

### 2026-05-04 - Etat initial

- Branche: `ayoub_tentative_mcts`.
- Fichier non suivi deja present: `Soutenance-IA.md`.
- Baseline lue dans log: `Player win ratio : 100%`, `IA win ratio : 0%`.

### 2026-05-04 - Tests locaux apres premiere implementation

- Compilation objet locale OK:
  - `g++ -std=c++17 -O2 -fno-lto -c Plateau.cpp -o /tmp/Plateau.o`
  - `g++ -std=c++17 -O2 -fno-lto -c main.cpp -o /tmp/main.o`
- Smoke test minimax par defaut OK:
  - premier coup: `4 4`
  - `resultatPartie()`: `99` (`PARTIE_CONTINUE`)
- Smoke test MCTS avec `-DACTIVER_MCTS` OK:
  - exemple: `[MCTS] sims=58567 sims/s=167334 temps_ms=350`
  - premier coup: `3 3`
  - `resultatPartie()`: `99` (`PARTIE_CONTINUE`)
- Campagne Windows non lancee ici: binaire Windows non executable depuis macOS, pas de Wine/MinGW local.
- Connexion VM testee ensuite: OK (`DESKTOP-5JAM424`, `desktop-5jam424\ayoub`).

### 2026-05-04 - Preparation bench VM

- Dossier separe cree sur VM:
  - `C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA-mcts-bench`
- Sources courantes copiees dans ce dossier.
- Builds Windows OK:
  - minimax par defaut: `ia_minimax.exe`
  - MCTS active: `ia_mcts.exe` avec `-DACTIVER_MCTS`
- Scripts batch prepares:
  - `run_minimax_med1.bat` -> log `run_mcts_minimax_med1.log`
  - `run_mcts_med1.bat` -> log `run_mcts_med1.log`
- Taches planifiees creees:
  - `UTTT_MCTS_MINIMAX_MED1`
  - `UTTT_MCTS_MED1`
- Blocage actuel:
  - `quser` indique aucune session console ouverte.
  - Les taches `/IT` ne demarrent pas sans session interactive.
  - L'execution directe SSH echoue comme prevu: `Failed to create display!`.
- Action humaine necessaire:
  - ouvrir/verrouiller une session Windows `ayoub` dans la VM, puis relancer `schtasks /Run`.

### 2026-05-05 - Campagnes MEDIUM_1 apres ouverture session VM

- Session console `ayoub` ouverte: `quser` OK.
- Campagne minimax par defaut:
  - binaire: `ia_minimax.exe`
  - log VM: `run_mcts_minimax_med1.log`
  - log local: `/Users/aubepine/Documents/Coding/Projet TicTacToe/test-logs/run_mcts_minimax_med1.log`
  - starts = 100
  - wins = 99
  - losses = 0
  - draws = 1
  - ratio lib: `Player win ratio : 99%`, `IA win ratio : 0%`
  - conclusion: fallback minimax toujours sain, aucune defaite.
- Campagne MCTS minimal:
  - binaire: `ia_mcts.exe` compile avec `-DACTIVER_MCTS`
  - log VM: `run_mcts_med1.log`
  - log local: `/Users/aubepine/Documents/Coding/Projet TicTacToe/test-logs/run_mcts_med1.log`
  - starts = 100
  - wins = 98
  - losses = 0
  - draws = 2
  - ratio lib: `Player win ratio : 98%`, `IA win ratio : 0%`
  - exemples de perf par coup vus en fin de log: environ 29k a 44k simulations en 350 ms, soit 82k a 126k sims/s.
  - conclusion: MCTS minimal ne regresse pas contre MEDIUM_1, mais il reste un peu moins stable que le minimax fige.
- Incident:
  - apres `Game: Finish`, les processus `ia_minimax.exe` et `ia_mcts.exe` etaient encore visibles dans `tasklist`.
  - ils ont ete arretes avec `taskkill /IM ia_minimax.exe /F` et `taskkill /IM ia_mcts.exe /F`.
  - a surveiller: la lib/GUI peut garder le process ouvert meme apres log `Finish`, donc toujours verifier `tasklist`.

### 2026-05-05 - Correction test MEDIUM_2 invalide

- Erreur detectee au moment de lancer HARD_1:
  - `main.cpp` utilisait `constexpr Level LEVEL = Level::MEDIUM_1`.
  - les arguments `med2` / `hard1` passes au `.exe` ne changeaient donc pas le niveau.
  - le log nomme `run_mcts_med2.log` contenait en fait `Game: - Level : Medium 1`.
- Decision:
  - ne pas utiliser ce log comme preuve MEDIUM_2.
  - supprimer `test-logs/run_mcts_med2.log` du repo pour eviter une preuve trompeuse.
  - ajouter une selection de niveau par macro de compilation dans `main.cpp`.
- Macros disponibles:
  - par defaut: `MEDIUM_1`
  - `-DNIVEAU_MEDIUM_2`: compile en `MEDIUM_2`
  - `-DNIVEAU_HARD_1`: compile en `HARD_1`

### 2026-05-05 - Campagne MCTS HARD_1

- Commit de base:
  - `76ce098` (`ajoute tentative mcts mesurable`)
- Binaire VM:
  - `ia_mcts_hard1.exe`, build avec `-DACTIVER_MCTS -DNIVEAU_HARD_1`
- Commande de campagne:
  - `ia_mcts_hard1.exe`
- Log VM:
  - `C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA-mcts-bench\run_mcts_hard1.log`
- Logs locaux:
  - `/Users/aubepine/Documents/Coding/Projet TicTacToe/test-logs/run_mcts_hard1.log`
  - `test-logs/run_mcts_hard1.log`
- Resultat:
  - starts = 100
  - wins = 1
  - losses = 93
  - draws = 6
  - ratio lib: `Player win ratio : 1%`, `IA win ratio : 93%`
- Performance observee:
  - niveau bien confirme par le log: `Game: - Level : Hard 1`.
- Conclusion:
  - MCTS minimal pur ne passe pas HARD_1.
  - Le probleme n'est pas seulement le nombre de simulations: les playouts aleatoires se font punir tactiquement.
  - Prochaine etape: ajouter un playout tactique leger (gagner une sous-grille si possible, bloquer une menace immediate, sinon aleatoire), puis retester MEDIUM_2 reel et HARD_1.

### 2026-05-05 - Campagne MCTS MEDIUM_2 reel, arret a 20 defaites

- Binaire VM:
  - `ia_mcts_med2.exe`, build avec `-DACTIVER_MCTS -DNIVEAU_MEDIUM_2`
- Niveau confirme par le log:
  - `Game: - Level : Medium 2`
- Commande:
  - `ia_mcts_med2.exe`
- Regle d'arret demandee:
  - stopper des que `loss >= 20`
- Logs:
  - VM: `C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA-mcts-bench\run_mcts_med2_reel.log`
  - local: `/Users/aubepine/Documents/Coding/Projet TicTacToe/test-logs/run_mcts_med2_reel_stop20loss.log`
  - repo: `test-logs/run_mcts_med2_reel_stop20loss.log`
- Resultat partiel au stop:
  - starts = 28
  - wins = 4
  - losses = 20
  - draws = 3
  - pas de `Finish`, car arret manuel a 20 defaites.
- Conclusion:
  - MCTS minimal pur ne valide pas MEDIUM_2 reel.
  - Le faux resultat precedent venait bien du niveau compile en `MEDIUM_1`.
  - Prochaine etape prioritaire: ajouter un playout tactique leger avant de refaire MEDIUM_2.

### 2026-05-05 - Test playout tactique naif MEDIUM_2 reel

- Changement teste localement mais non conserve:
  - garde-fou avant MCTS: si coup gagnant ou blocage immediat, le jouer directement.
  - playout MCTS: gagner une sous-grille si possible, sinon bloquer une victoire locale adverse, sinon aleatoire.
- Binaire VM:
  - `ia_mcts_med2.exe`, build avec `-DACTIVER_MCTS -DNIVEAU_MEDIUM_2`
- Niveau confirme:
  - `Game: - Level : Medium 2`
- Regle d'arret:
  - stopper des que `loss >= 20`
- Logs:
  - VM: `C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA-mcts-bench\run_mcts_med2_tactique_stop20loss.log`
  - local: `/Users/aubepine/Documents/Coding/Projet TicTacToe/test-logs/run_mcts_med2_tactique_stop20loss.log`
  - repo: `test-logs/run_mcts_med2_tactique_stop20loss.log`
- Resultat partiel:
  - starts = 22
  - wins = 0
  - losses = 21
  - draws = 0
- Conclusion:
  - regression nette vs MCTS minimal (qui faisait 4 wins / 20 losses / 3 draws au stop).
  - le playout tactique naif a ete retire du code.
  - hypothese: gagner/bloquer une sous-grille localement sans regarder la grille envoyee donne trop souvent de bons coups locaux mais de mauvais coups UTTT.
  - prochaine piste plus credible: hybride simple, par exemple utiliser minimax existant comme garde-fou racine ou comme selection parmi les coups les plus visites.

### 2026-05-05 - Test MCTS-EH DeepSearch MEDIUM_2 reel

- Changement teste localement mais non conserve:
  - suppression des playouts aleatoires.
  - evaluation directe du plateau etendu avec `evaluer()`.
  - normalisation sigmoid avec temperature `8000.0`.
- Idee DeepSearch:
  - garder selection/expansion MCTS.
  - remplacer la simulation par l'heuristique minimax existante.
- Binaire VM:
  - `ia_mcts_med2.exe`, build avec `-DACTIVER_MCTS -DNIVEAU_MEDIUM_2`
- Niveau confirme:
  - `Game: - Level : Medium 2`
- Regle d'arret:
  - kill switch DeepSearch: stopper si `loss >= 8` avant ou a 15 parties.
- Logs:
  - VM: `C:\Users\ayoub\projets\Projet\TicTacToeAI-AGA-mcts-bench\run_mcts_med2_eval_stop.log`
  - local: `/Users/aubepine/Documents/Coding/Projet TicTacToe/test-logs/run_mcts_med2_eval_stop_kill.log`
  - repo: `test-logs/run_mcts_med2_eval_stop_kill.log`
- Resultat partiel:
  - starts = 9
  - wins = 0
  - losses = 8
  - draws = 0
- Conclusion:
  - regression immediate, kill switch declenche.
  - MCTS-EH tel quel a ete retire du code.
  - cause probable: l'evaluation statique utilisee seule transforme MCTS en exploration de coups a horizon tres court, sans assez de tactique adversaire.

## Implementation en cours

- Ajout de `resultatPartie()`:
  - retourne `1` si joueur gagne,
  - retourne `-1` si IA gagne,
  - retourne `0` si egalite,
  - retourne `99` si la partie continue.
- Ajout d'un MCTS minimal derriere un switch compile-time:
  - defaut: minimax stable,
  - MCTS: compiler avec `-DACTIVER_MCTS`.
- Le MCTS actuel est volontairement simple:
  - simulations aleatoires,
  - UCB1,
  - choix final par enfant le plus visite,
  - logs `sims`, `sims/s`, `temps_ms`.
- Note importante:
  - le playout tactique naif a ete teste puis rollback car il regressait fortement MEDIUM_2 reel.
  - MCTS-EH par evaluation directe `evaluer()` a ete teste puis rollback car il declenche le kill switch MEDIUM_2.

## Notes pour reprise

- Ne pas toucher a la version stable sans garder un fallback clair.
- Eviter les bitboards au debut.
- Garder les commentaires courts et pedagogiques.
- Les logs doivent aider a expliquer, pas noyer la sortie.
- Prochaine vraie etape: tester un hybride simple MCTS/minimax, pas relancer le playout tactique naif tel quel.
