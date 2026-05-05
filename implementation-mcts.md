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

## Notes pour reprise

- Ne pas toucher a la version stable sans garder un fallback clair.
- Eviter les bitboards au debut.
- Garder les commentaires courts et pedagogiques.
- Les logs doivent aider a expliquer, pas noyer la sortie.
- Prochaine vraie etape: lancer une campagne VM courte minimax sans `-DACTIVER_MCTS`, puis MCTS avec `-DACTIVER_MCTS`.
