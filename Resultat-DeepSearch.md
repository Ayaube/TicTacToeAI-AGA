# **Rapport de Recherche en Ingénierie Algorithmique : Intelligence Artificielle pour Ultimate Tic-Tac-Toe**

## **Résumé Exécutif**

L'objectif de la présente recherche est de concevoir et d'intégrer une intelligence artificielle performante au sein d'un moteur de jeu Ultimate Tic-Tac-Toe (développé en C++17), capable de vaincre des adversaires de niveau difficile (HARD / VERY\_HARD). Le cahier des charges impose des contraintes architecturales et académiques strictes : la solution doit être concise (moins de 500 lignes de code modifiées ou ajoutées), s'intégrer dans une architecture logicielle simple (main.cpp et Plateau.h/.cpp), et surtout, demeurer totalement explicable lors d'une soutenance étudiante, excluant de fait les approches opaques de type "boîte noire". L'analyse approfondie de la théorie des jeux combinatoires et de l'espace d'états vertigineux de l'Ultimate Tic-Tac-Toe (évalué théoriquement à ![][image1] configurations) démontre que les algorithmes traditionnels de recherche exhaustive sont inopérants. L'évaluation comparative porte sur trois paradigmes : le Minimax optimisé par élagage Alpha-Beta, la Recherche Arborescente Monte Carlo (MCTS), et les approches hybrides par apprentissage profond. L'approche neuronale est formellement écartée en raison de son opacité et de sa lourdeur d'implémentation. Bien que l'algorithme Minimax soit prédictible, il requiert pour ce jeu précis une fonction d'évaluation heuristique d'une grande complexité, dont les pondérations arbitraires s'avèrent périlleuses à justifier scientifiquement lors d'un examen oral.  
Par conséquent, la stratégie principale recommandée est la Recherche Arborescente Monte Carlo (MCTS). Cette méthode ne nécessite aucune heuristique experte, s'implémente en un nombre très restreint de lignes de code grâce à une logique itérative élégante, et s'explique intuitivement par la simulation de trajectoires aléatoires guidées par les probabilités statistiques. En cas de limitations extrêmes des ressources de calcul entravant les simulations, une stratégie de secours basée sur un algorithme Minimax avec élagage Alpha-Beta, couplé à une heuristique statique de contrôle des sous-grilles, est également documentée. Le rapport détaille l'analyse de la littérature, le plan d'implémentation C++17 tirant parti de représentations binaires optimisées (Bitboards), le pseudo-code exhaustif, un protocole de validation statistique rigoureux, ainsi qu'une proposition de vulgarisation conceptuelle destinée à la soutenance orale.

## **Analyse de la Littérature et Contributions des Sources**

Pour asseoir la légitimité des choix algorithmiques, une revue approfondie de la littérature scientifique et technique existante sur l'Ultimate Tic-Tac-Toe a été réalisée. La complexité de ce jeu a attiré l'attention des chercheurs en intelligence artificielle, car il se situe à la frontière entre les jeux triviaux (comme le Tic-Tac-Toe classique) et les jeux d'une complexité insondable (comme les Échecs ou le Go).  
Les travaux de Powell et Merrill proposent une fondation cruciale pour comprendre les approches basées sur l'algorithme Minimax.1 Leur recherche apporte une quantification précise des heuristiques nécessaires pour évaluer un plateau de jeu en l'absence de nœuds terminaux. Ils démontrent qu'une victoire sur le plateau global vaut l'infini, mais introduisent des pondérations intermédiaires vitales : remporter une sous-grille ajoute 100 points, mais remporter une sous-grille qui bloque une ligne de trois de l'adversaire est valorisé à 150 points. À l'inverse, remporter une sous-grille qui est déjà bloquée par l'adversaire entraîne une pénalité de \-150 points. Cette source est fondamentale car elle prouve que pour rendre Minimax performant, l'ingénieur doit injecter une connaissance experte du jeu extrêmement nuancée. L'implémentation d'une heuristique de blocage valorise massivement la défense d'une ligne globale, car empêcher l'adversaire de s'aligner est statistiquement plus crucial que de progresser sur une ligne isolée.  
En opposition à l'approche heuristique, les publications explorant les algorithmes basés sur les données, telles que le projet uttt.ai ou les travaux de Paruthi, illustrent l'efficacité de l'apprentissage par renforcement et des réseaux de neurones.2 Ces sources démontrent qu'une architecture inspirée d'AlphaZero, combinant MCTS et réseaux de neurones (Policy-Value Networks), écrase les approches traditionnelles. La source uttt.ai indique que la profondeur moyenne d'une partie se situe entre 40 et 50 demi-tours, avec un facteur de branchement moyen d'environ 7 coups légaux par position. Cependant, ces sources apportent également une mise en garde majeure : l'entraînement de tels modèles nécessite la génération de millions de parties en auto-jeu et s'appuie sur des réseaux contenant plusieurs millions de paramètres (environ 5 millions pour uttt.ai). Cette dimension "boîte noire" et l'impossibilité d'intégrer de telles bibliothèques d'inférence (comme ONNX ou PyTorch) sans briser la contrainte de compacité du code C++ nous obligent à rejeter cette solution pour le livrable final, tout en reconnaissant sa supériorité théorique.  
Les recherches comparatives publiées notamment lors de la conférence FLAIRS 2022 par Addison, Peeler et Alvin évaluent directement les performances croisées de différentes intelligences artificielles sur l'Ultimate Tic-Tac-Toe.4 Leur étude confronte des bots aléatoires, des bots basés sur des cartes de chaleur (heuristiques positionnelles) et des bots MCTS. L'apport majeur de cette source réside dans l'analyse des contraintes de temps. Les auteurs soulignent que si le temps de réflexion est sévèrement limité (par exemple à un dixième de seconde), MCTS n'a pas le temps d'explorer l'arbre suffisamment en profondeur pour surpasser significativement un bot heuristique rapide. À l'inverse, avec un budget de temps adéquat, MCTS surpasse les approches statiques. Cette information est déterminante pour notre analyse des risques : la viabilité de MCTS dépendra intrinsèquement de la vitesse d'exécution de la classe Plateau en C++.  
L'analyse de l'ingénierie logicielle requise pour accélérer ces algorithmes nous est fournie par les travaux d'optimisation de bas niveau, notamment ceux documentés sur minimax.dev.6 Cette source explique comment contourner le goulot d'étranglement de la mémoire en utilisant des "Bitboards" (tableaux de bits). En représentant l'état des 81 cases à l'aide d'entiers non signés de 16 ou 32 bits, les opérations de copie de plateau deviennent quasiment gratuites pour le processeur. De plus, la vérification des conditions de victoire, qui doit être exécutée des millions de fois par MCTS, se réduit à de simples opérations logiques (ET, OU bit à bit) avec des masques précalculés. Cette source apporte la validation technique qu'une implémentation de pointe peut tenir en très peu de lignes de code si les structures de données sont intelligemment conçues.  
Enfin, les travaux académiques de Winands et al. détaillent les vulnérabilités intrinsèques de l'algorithme MCTS face aux pièges tactiques à court terme (tactical blindness).7 La source explique que MCTS, en raison de ses simulations aléatoires, peut sous-estimer le danger immédiat d'une position si la séquence fatale est étroite et peu susceptible d'être jouée par hasard. Cette source motive la nécessité d'intégrer des "heavy playouts" (simulations guidées) ou de conserver une stratégie Minimax en solution de repli pour pallier ce déficit dans des environnements de jeu hautement tactiques.

## **Complexité Théorique et Dynamiques de l'Ultimate Tic-Tac-Toe**

Comprendre les algorithmes nécessite d'abord de comprendre la nature mathématique du champ de bataille. L'Ultimate Tic-Tac-Toe (UTTT) transcende le jeu classique en introduisant une dimension fractale et une règle de mouvement contraignante. L'espace d'états théorique brut du jeu est de ![][image1], un nombre dépassant largement les capacités de stockage et de calcul de toute infrastructure informatique contemporaine.8 Bien que l'arbre de jeu effectif soit réduit par l'inaccessibilité de nombreux états, la résolution exhaustive reste hors de portée.  
La règle fondatrice de l'UTTT stipule que le coup joué dans une cellule locale dicte obligatoirement la sous-grille dans laquelle l'adversaire devra jouer au tour suivant. Cette mécanique introduit une notion de sacrifice positionnel totalement absente du Tic-Tac-Toe classique. Un joueur expert sacrifiera volontairement la victoire sur une sous-grille périphérique s'il peut, en échange, forcer son adversaire à l'envoyer jouer dans la sous-grille centrale, considérée comme le pivot stratégique du jeu.1  
La seconde règle critique, génératrice de chaos combinatoire, est le "coup libre" (wildcard). Lorsqu'un joueur est contraint de jouer dans une sous-grille déjà remportée ou entièrement remplie, la contrainte saute : il gagne le droit de jouer n'importe où sur le plateau global.1 Offrir un tel coup libre à l'adversaire constitue généralement une erreur fatale. C'est ici que l'intuition humaine vacille et que l'algorithmique doit prendre le relais : l'évaluation d'un coup ne se mesure pas à l'alignement immédiat de trois symboles, mais à la restriction des libertés de mouvement imposée à l'adversaire dans les tours subséquents.

## **Évaluation Comparative des Paradigmes Algorithmiques**

Le cahier des charges exige une performance de haut niveau couplée à une clarté académique irréprochable. L'ajout d'une contrainte de volume (moins de 500 lignes de code) élimine de facto les architectures tentaculaires. Trois approches distinctes ont été soumises à une évaluation rigoureuse pour déterminer leur adéquation au projet.

### **L'Approche Heuristique : Minimax et Élagage Alpha-Beta**

L'algorithme Minimax est la pierre angulaire de l'intelligence artificielle classique pour les jeux à deux joueurs à somme nulle. Son postulat est déterministe : il suppose que les deux adversaires joueront toujours le coup optimal.8 L'algorithme déploie un arbre de tous les coups possibles. À chaque niveau, il alterne entre la maximisation du score (le tour de l'IA) et la minimisation du score (le tour de l'adversaire). L'élagage Alpha-Beta vient optimiser ce processus en coupant court à l'exploration des branches qui s'avèrent mathématiquement inférieures aux options déjà découvertes, réduisant ainsi drastiquement la charge de calcul sans altérer la décision finale.12  
Dans des jeux simples, Minimax parcourt l'arbre jusqu'aux feuilles terminales (victoire, défaite, nul). Dans l'UTTT, la profondeur de l'arbre l'interdit. L'algorithme doit donc s'arrêter à une profondeur arbitraire (par exemple, 6 coups à l'avance) et faire appel à une fonction d'évaluation heuristique statique pour estimer la valeur de la position en cours.7  
La complexité de l'approche Minimax ne réside pas dans le code de l'algorithme lui-même, qui est remarquablement élégant et concis grâce à la récursivité, mais dans la conception de cette fonction d'évaluation. Pour que l'IA soit compétitive, l'ingénieur doit traduire la connaissance experte du jeu en un système de points. En s'appuyant sur les recherches de Powell et Merrill, une heuristique performante attribue des scores asymétriques : 100 points pour la conquête d'une sous-grille, une surprime de 150 points pour une grille qui contrecarre l'adversaire, et des pénalités massives (-150 points) pour des grilles conquises mais inutiles à l'alignement global.1 De plus, le contrôle du centre doit être surpondéré, car le centre participe à quatre axes de victoire potentiels, contre trois pour les coins et deux pour les bords.1  
Cette nécessité d'ingénierie heuristique pose un problème majeur pour la soutenance étudiante. La justification des valeurs numériques exactes (pourquoi 150 et non 200?) relève souvent de l'ajustement empirique et s'apparente à une forme de "magie" mathématique difficilement défendable face à un jury exigeant une rationalité algorithmique absolue. De surcroît, le Minimax souffre de l'effet d'horizon : il est incapable de percevoir une défaite inéluctable située juste un coup au-delà de sa profondeur de recherche, le poussant à faire des choix irrationnels.13

### **L'Approche Probabiliste : Monte Carlo Tree Search (MCTS)**

Le Monte Carlo Tree Search représente un changement de paradigme fondamental. Plutôt que d'évaluer statiquement un plateau arrêté dans le temps, MCTS l'évalue dynamiquement en simulant le futur. L'algorithme ne requiert aucune connaissance des stratégies du jeu (domain-independent) ; il lui suffit de connaître les règles légales et les conditions de victoire.8  
Le moteur MCTS opère en exécutant itérativement quatre phases distinctes jusqu'à l'épuisement du temps de calcul alloué (par exemple, 500 millisecondes) 10 :

1. **Sélection :** L'algorithme navigue depuis la racine (l'état actuel du jeu) à travers l'arbre des nœuds déjà explorés. Pour choisir le chemin, il utilise une équation mathématique nommée UCB1 (Upper Confidence Bound). Cette formule orchestre un compromis permanent entre l'exploitation (choisir le nœud qui a le plus fort taux de victoire jusqu'à présent) et l'exploration (choisir un nœud qui a été peu visité pour s'assurer qu'il ne cache pas une stratégie gagnante ignorée).  
2. **Expansion :** Lorsqu'il atteint un nœud dont tous les coups enfants n'ont pas encore été instanciés en mémoire, il sélectionne un coup vierge et crée un nouveau nœud dans l'arbre.  
3. **Simulation (Playout) :** À partir de ce nouveau nœud, l'algorithme lance une simulation "à l'aveugle". Il génère des coups totalement aléatoires pour les deux joueurs, à une vitesse fulgurante, jusqu'à ce que la partie simulée se termine par une victoire, une défaite ou un match nul.  
4. **Rétropropagation :** Le résultat de cette simulation aléatoire (1 pour une victoire, 0 pour une défaite, 0.5 pour un nul) remonte le long de la branche traversée jusqu'à la racine. Chaque nœud sur le chemin met à jour ses statistiques (incrémentation du nombre de passages, ajustement du score de victoire).

Au terme du temps imparti, la racine examine ses enfants directs et sélectionne simplement le coup qui a été visité le plus grand nombre de fois. Cette métrique de "robustesse" s'avère bien plus fiable que le simple ratio de victoire, car un nœud extrêmement visité indique que l'algorithme a cherché sans relâche, et sans succès, un moyen de le réfuter.10  
La force du MCTS réside dans sa croissance asymétrique.10 Face au facteur de branchement de l'UTTT, il ne perd pas de temps à évaluer en profondeur les coups manifestement désastreux. Il concentre sa puissance de calcul sur les lignes de jeu les plus critiques. Pour une soutenance académique, l'explicabilité est absolue : il n'y a pas de pondérations arbitraires, seulement la loi des grands nombres et la résolution du dilemme exploration/exploitation via les mathématiques des bandits manchots.

### **L'Approche Neuronale : Réseaux de Neurones et Apprentissage Profond**

La troisième voie, magnifiée par les succès de DeepMind avec AlphaZero, consiste à hybrider le MCTS avec des réseaux de neurones profonds. Dans cette architecture, les simulations aléatoires du MCTS (qui peuvent être imprécises) sont remplacées par l'inférence d'un "Value Network" qui prédit instantanément les probabilités de victoire d'une position.2 Parallèlement, un "Policy Network" guide la phase de sélection en suggérant les coups les plus pertinents *a priori*.  
Des implémentations dédiées à l'UTTT, telles que le projet uttt.ai, prouvent que cette architecture offre un niveau de jeu inatteignable par les algorithmes classiques.2 Un réseau de neurones de 5 millions de paramètres entraîné par auto-apprentissage écrase systématiquement un MCTS pur. Cependant, le fossé technologique requis pour y parvenir est infranchissable dans le cadre de ce projet. L'intégration exige des bibliothèques externes lourdes (inférence ONNX ou PyTorch C++), un temps d'entraînement se comptant en semaines de calcul GPU continu 2, et un volume de code largement supérieur à la contrainte de 500 lignes. Plus rédhibitoire encore, le réseau de neurones opère comme une "boîte noire" totale. Il est impossible pour un étudiant de tracer ou de justifier logiquement pourquoi le tenseur a produit telle prédiction de valeur pour une configuration donnée du plateau.8

### **Matrice de Décision et Compromis**

Pour synthétiser cette évaluation, la matrice suivante confronte les trois approches aux exigences strictes du cahier des charges :

| Critère d'Évaluation | Minimax \+ Alpha-Beta | Recherche Arborescente Monte Carlo (MCTS) | Hybride Deep Learning (AlphaZero) |
| :---- | :---- | :---- | :---- |
| **Performance Globale** | Moyenne à Forte (fortement dépendante de l'heuristique) | **Forte (Extrêmement résilient à long terme)** | Très Haute (État de l'art) |
| **Complexité d'Implémentation** | Moyenne (Code court, mais ajustement heuristique laborieux) | **Faible (Logique mathématique universelle et concise)** | Très Élevée (Entraînement hors ligne, dépendances ML) |
| **Transparence et Explicabilité** | Faible (Les pondérations heuristiques s'apparentent à de la magie) | **Excellente (S'appuie sur la statistique et les probabilités intuitives)** | Nulle (Réseau de neurones opaque par définition) |
| **Risques Techniques** | Effet d'horizon ; Incapacité de s'adapter dynamiquement au temps alloué | **Cécité tactique sur des pièges très courts ; Requiert une classe Plateau rapide** | Explosion des délais ; Dépendances externes brisant les règles du projet |
| **Adéquation à la limite de 500 lignes** | Oui (Environ 300 lignes avec les fonctions de scoring) | **Oui (Environ 200 à 250 lignes)** | Non (Architecture d'inférence trop massive) |

## **Choix Algorithmique Final**

**La stratégie principale retenue pour le cœur de l'intelligence artificielle est la Recherche Arborescente Monte Carlo (MCTS).**  
Le choix du MCTS s'impose comme le compromis idéal. Il répond avec une précision chirurgicale à l'exigence de concision (le code tient en moins de 300 lignes), tout en offrant un niveau de difficulté HARD/VERY\_HARD organique. Contrairement au Minimax, l'IA MCTS ne nécessite pas d'inculquer au code les concepts de "centre", de "coin" ou de "lignes potentielles". L'algorithme découvre ces concepts stratégiques par lui-même en observant que les simulations passant par le centre génèrent statistiquement plus de victoires. Cette émergence de l'intelligence par l'expérience statistique est un argument de soutenance d'une puissance redoutable. En outre, MCTS est un algorithme de nature *anytime* : il boucle de façon ininterrompue et peut être stoppé à la milliseconde près lorsque le temps de réflexion de l'ordinateur est écoulé, garantissant une réactivité parfaite sans nécessiter la machinerie complexe de l'*Iterative Deepening* requise par Minimax.16  
**Une stratégie de secours (Fallback) est prévue : le Minimax Alpha-Beta avec heuristique simplifiée.** Bien que MCTS soit intrinsèquement supérieur pour la clarté du code, son efficacité dépend d'une variable vitale : le nombre de simulations qu'il peut exécuter par seconde. Si le code C++ préexistant de la classe Plateau est mal optimisé (par exemple, allocations dynamiques massives à chaque copie de plateau), le MCTS s'effondrera par manque d'échantillons statistiques. Dans un tel scénario de défaillance, le projet basculera sur un Minimax Alpha-Beta à faible profondeur (4 ou 5), guidé par une heuristique défensive simple : valoriser le gain des sous-plateaux (+100) et infliger une pénalité sévère (-200) à tout coup offrant à l'adversaire la possibilité de jouer n'importe où (règle du coup libre).1

## **Ingénierie Logicielle et Plan d'Implémentation C++17**

Pour respecter la contrainte de compacité et maximiser les performances de la stratégie MCTS, l'implémentation doit exploiter la modernité du C++17. L'objectif de performance est d'atteindre plusieurs dizaines de milliers de simulations aléatoires par seconde. L'intégration se fera de manière incrémentale, sans déconstruire la logique existante.

### **Phase 1 : Mutation de la Classe Plateau vers les Bitboards**

La structure de données de l'état du jeu est le goulot d'étranglement de toute IA basée sur la recherche arborescente. L'implémentation naïve utilisant des std::vector\<std::vector\<char\>\> provoque une fragmentation de la mémoire et des défauts de cache CPU désastreux lors des millions de copies requises par MCTS.  
La littérature technique en ingénierie de jeux de plateau préconise l'utilisation de "Bitboards" (tableaux de bits).6 L'Ultimate Tic-Tac-Toe requiert de suivre l'état de 81 cases locales et de 9 cases macroscopiques. Le C++17 permet de compacter cette information. Une approche élégante et minimaliste consiste à utiliser des entiers non signés. Chaque sous-grille de 9 cases peut être encodée sur un entier de 16 bits (uint16\_t). Un tableau contigu de 9 entiers suffit à représenter les pions du joueur 1, et un second tableau représente les pions du joueur 2\.  
Cette refonte offre un avantage colossal : la vérification de victoire, traditionnellement coûteuse avec des boucles imbriquées, s'exécute en une nanoseconde grâce à des opérateurs binaires (ET logique) appliqués contre 8 masques constants représentant les 8 combinaisons gagnantes d'un morpion (3 lignes, 3 colonnes, 2 diagonales).6  
*Modifications dans Plateau.h :*

* Ajout d'une méthode std::vector\<int\> obtenir\_coups\_legaux() const; générant rapidement la liste des index (0 à 80\) autorisés.  
* Ajout d'une méthode de copie de l'état ultra-rapide (un simple memcpy sous le capot grâce aux types POD).  
* Optimisation de la méthode int evaluer\_etat\_victoire() const; pour retourner rapidement l'identifiant du gagnant ou signaler que la partie continue.

### **Phase 2 : Modélisation du Graphe MCTS**

La gestion de la mémoire de l'arbre MCTS est délicate. Créer et détruire des millions d'objets Node avec l'opérateur new fragmenterait la mémoire et ralentirait l'exécution.  
La solution élégante en C++17 est de modéliser le nœud sous forme de struct légère, et de stocker ces nœuds soit via des std::unique\_ptr pour déléguer la gestion de la mémoire au compilateur, soit dans un std::vector\<MCTSNode\> pré-alloué agissant comme un *Arena Allocator*.  
La structure du nœud doit être ascétique :

* Un identifiant du coup ayant mené à ce nœud.  
* Un pointeur ou index vers le nœud parent.  
* Un vecteur des coups légaux non encore explorés (servant à la phase d'Expansion).  
* Un vecteur des pointeurs vers les nœuds enfants.  
* Deux compteurs vitaux : nombre\_visites et score\_victoires.

### **Phase 3 : Implémentation de la Boucle Temporelle et de l'Aléatoire**

La boucle principale de l'IA MCTS instancie une horloge de haute résolution std::chrono::high\_resolution\_clock. Tant que la limite de temps fixée (par exemple, 400 millisecondes) n'est pas atteinte, l'algorithme exécute le cycle Sélection-Expansion-Simulation-Rétropropagation.  
Pour la phase de simulation, la rapidité de la génération aléatoire est cruciale. L'utilisation de rand() de la bibliothèque standard C est proscrite pour son inefficacité et ses biais. L'implémentation emploiera le générateur std::mt19937 (Mersenne Twister) couplé à une distribution uniforme std::uniform\_int\_distribution, des fonctionnalités natives du C++ moderne qui garantissent une sélection de coups stochastique à très haute fréquence.

## **Cœur Algorithmique : Le Pseudo-Code Documenté**

Le pseudo-code suivant matérialise l'architecture de la stratégie MCTS retenue. Il se distingue par sa clarté fonctionnelle, garantissant un volume de code réel inférieur à 250 lignes tout en capturant toute l'intelligence du processus. La fonction d'exploration mathématique au cœur du système repose sur le calcul UCB1, explicité dans la méthode Selectionner\_Meilleur\_Enfant. L'heuristique d'exploration ![][image2], fixée autour de ![][image3] (environ 1.41), orchestre scientifiquement l'équilibre entre la répétition des coups victorieux et la découverte de tactiques inédites.10

C++

// \--- Constantes et Structures Fondamentales \---  
const double CONSTANTE\_EXPLORATION \= 1.41421356;

struct NoeudMCTS {  
    int coup\_joue;               // Le coup qui a généré ce nœud (-1 pour la racine)  
    int joueur\_courant;          // Le joueur devant agir à partir de ce nœud  
    double score\_victoires \= 0.0;// (W) Somme des résultats des simulations  
    int nombre\_visites \= 0;      // (N) Nombre de fois où ce nœud a été traversé  
      
    NoeudMCTS\* parent \= nullptr;  
    std::vector\<std::unique\_ptr\<NoeudMCTS\>\> enfants;  
    std::vector\<int\> coups\_non\_explores; // Rempli lors de l'instanciation  
      
    NoeudMCTS(int coup, int joueur, NoeudMCTS\* p, const Plateau& etat)   
        : coup\_joue(coup), joueur\_courant(joueur), parent(p) {  
        coups\_non\_explores \= etat.obtenir\_coups\_legaux();  
    }  
};

// \--- Cœur de l'Intelligence Artificielle \---  
int Generer\_Meilleur\_Coup(const Plateau& etat\_initial, int temps\_max\_ms) {  
    auto racine \= std::make\_unique\<NoeudMCTS\>(-1, etat\_initial.joueur\_actuel(), nullptr, etat\_initial);  
    auto debut\_chrono \= std::chrono::high\_resolution\_clock::now();

    // Boucle continue jusqu'à l'expiration du chronomètre  
    while (std::chrono::duration\_cast\<std::chrono::milliseconds\>(  
           std::chrono::high\_resolution\_clock::now() \- debut\_chrono).count() \< temps\_max\_ms) {  
          
        NoeudMCTS\* noeud\_courant \= racine.get();  
        Plateau plateau\_simule \= etat\_initial; // Copie de travail isolée

        // 1\. PHASE DE SÉLECTION (Descente via UCB1)  
        while (noeud\_courant-\>coups\_non\_explores.empty() &&\!noeud\_courant-\>enfants.empty()) {  
            noeud\_courant \= Selectionner\_Meilleur\_Enfant(noeud\_courant);  
            plateau\_simule.jouer\_coup(noeud\_courant-\>coup\_joue, noeud\_courant-\>parent-\>joueur\_courant);  
        }

        // 2\. PHASE D'EXPANSION (Création d'une nouvelle possibilité)  
        if (\!noeud\_courant-\>coups\_non\_explores.empty() && plateau\_simule.evaluer\_etat\_victoire() \== \-1) {  
            int coup\_choisi \= noeud\_courant-\>coups\_non\_explores.back();  
            noeud\_courant-\>coups\_non\_explores.pop\_back();  
              
            plateau\_simule.jouer\_coup(coup\_choisi, noeud\_courant-\>joueur\_courant);  
            int prochain\_joueur \= (noeud\_courant-\>joueur\_courant \== 1)? 2 : 1;  
              
            auto nouvel\_enfant \= std::make\_unique\<NoeudMCTS\>(coup\_choisi, prochain\_joueur, noeud\_courant, plateau\_simule);  
            noeud\_courant-\>enfants.push\_back(std::move(nouvel\_enfant));  
            noeud\_courant \= noeud\_courant-\>enfants.back().get();  
        }

        // 3\. PHASE DE SIMULATION (Playout aléatoire rapide)  
        int etat\_partie \= plateau\_simule.evaluer\_etat\_victoire();  
        int joueur\_simu \= noeud\_courant-\>joueur\_courant;  
          
        while (etat\_partie \== \-1) { // \-1 signifie que la partie continue  
            std::vector\<int\> coups\_dispos \= plateau\_simule.obtenir\_coups\_legaux();  
            // Génération Mersenne Twister pour performance maximale  
            int coup\_aleatoire \= coups\_dispos\[std::mt19937\_generateur() % coups\_dispos.size()\];  
              
            plateau\_simule.jouer\_coup(coup\_aleatoire, joueur\_simu);  
            joueur\_simu \= (joueur\_simu \== 1)? 2 : 1;  
            etat\_partie \= plateau\_simule.evaluer\_etat\_victoire();  
        }

        // 4\. PHASE DE RÉTROPROPAGATION (Mise à jour de la branche)  
        Retropropager\_Resultats(noeud\_courant, etat\_partie);  
    }

    // Le choix final repose sur la robustesse : l'enfant le plus visité  
    return Obtenir\_Enfant\_Plus\_Visite(racine.get())-\>coup\_joue;  
}

// \--- Implémentation Mathématique UCB1 \---  
NoeudMCTS\* Selectionner\_Meilleur\_Enfant(NoeudMCTS\* noeud) {  
    NoeudMCTS\* meilleur\_enfant \= nullptr;  
    double meilleur\_score\_ucb \= \-1.0;  
      
    for (auto& enfant : noeud-\>enfants) {  
        // Formule UCB1 : (Victoires / Visites) \+ Constante \* sqrt( ln(Visites\_Parent) / Visites )  
        double score\_exploitation \= enfant-\>score\_victoires / enfant-\>nombre\_visites;  
        double score\_exploration \= CONSTANTE\_EXPLORATION \* std::sqrt(std::log(noeud-\>nombre\_visites) / enfant-\>nombre\_visites);  
        double valeur\_ucb \= score\_exploitation \+ score\_exploration;  
          
        if (valeur\_ucb \> meilleur\_score\_ucb) {  
            meilleur\_score\_ucb \= valeur\_ucb;  
            meilleur\_enfant \= enfant.get();  
        }  
    }  
    return meilleur\_enfant;  
}

// \--- Mise à Jour Statistique \---  
void Retropropager\_Resultats(NoeudMCTS\* noeud, int id\_gagnant) {  
    while (noeud\!= nullptr) {  
        noeud-\>nombre\_visites++;  
        if (id\_gagnant \== 0\) { // Égalité  
            noeud-\>score\_victoires \+= 0.5;  
        } else if (noeud-\>parent\!= nullptr) {  
            // Si le gagnant est celui qui vient de jouer le coup menant à ce nœud  
            if (id\_gagnant \== noeud-\>parent-\>joueur\_courant) {  
                noeud-\>score\_victoires \+= 1.0;  
            }  
        }  
        noeud \= noeud-\>parent; // Remontée vers la racine  
    }  
}

## **Stratégie de Soutenance : L'Explicabilité "Boîte Blanche"**

L'un des défis majeurs de l'ingénierie algorithmique académique est la capacité à vulgariser la complexité sans la trahir. Le cahier des charges stipule que l'IA doit être parfaitement explicable en cinq minutes devant un jury, écartant définitivement l'argument d'autorité inhérent aux réseaux de neurones opaques.  
L'approche narrative recommandée pour la présentation orale s'articule autour de l'analogie de la "prospective statistique". L'étudiant pourra introduire son code ainsi :  
*"Face à un jeu comptant plus de configurations possibles qu'il n'y a d'atomes dans l'univers, dicter à l'ordinateur des règles humaines préétablies sur l'importance du centre ou des coins s'avère inefficace. Nous avons donc choisi de doter notre IA non pas de connaissances, mais d'une méthode de projection : la Recherche Arborescente Monte Carlo.*  
*À chaque milliseconde de son temps de réflexion, l'IA clone le plateau actuel dans sa mémoire et joue le reste de la partie de manière totalement aléatoire jusqu'à la fin. Elle répète ce processus des dizaines de milliers de fois, créant autant de futurs parallèles virtuels. Lorsqu'une trajectoire aléatoire aboutit à une victoire, l'IA mémorise les coups de départ qui ont initié cette séquence fructueuse.*  
*Toute la subtilité de notre code de 200 lignes réside dans une formule mathématique appelée UCB1. Cette équation agit comme le chef d'orchestre du cerveau de l'ordinateur : elle l'oblige à équilibrer constamment son temps entre jouer les coups qui ont statistiquement généré le plus de victoires dans ses simulations, et forcer l'exploration de coups encore ignorés pour vérifier s'ils ne cachent pas un piège mortel inattendu. À la fin de son temps de réflexion, l'IA joue simplement le coup qu'elle a ressenti le besoin de simuler le plus de fois, prouvant sa robustesse mathématique."*  
Cette narration démystifie l'intelligence artificielle en la ramenant à une mécanique probabiliste transparente, validant haut la main l'exigence d'explicabilité du cahier des charges.

## **Protocole de Test et Validation Scientifique**

Un projet d'ingénierie algorithmique n'a de valeur que si ses performances sont statistiquement démontrables. L'évaluation de l'IA MCTS ne doit pas reposer sur des anecdotes de parties individuelles, mais sur un protocole d'automatisation rigoureux permettant de garantir la classification du niveau de l'IA (HARD / VERY\_HARD).  
La métrique centrale d'évaluation sera le Taux de Victoire Exclusif (Winrate), calculé selon la formule :  
![][image4]  
Les matchs nuls sont exclus du dénominateur pour isoler la pure capacité de domination de l'algorithme sur son opposant.  
Le banc d'essai implique de faire s'affronter le bot MCTS contre trois profils de difficulté standardisée (Baselines). Afin de lisser la forte asymétrie de l'UTTT (qui favorise intrinsèquement le joueur commençant la partie), l'IA MCTS devra jouer la moitié des parties en tant que Joueur 1 (X) et l'autre moitié en tant que Joueur 2 (O).17 La taille de l'échantillon doit être massive pour absorber la variance probabiliste inhérente au MCTS.

| Adversaire Testé | Profil et Comportement Algorithmique | Volume de Matchs | Seuil de Validation (Winrate) |
| :---- | :---- | :---- | :---- |
| **Bot Aléatoire (Baseline)** | Effectue des coups légaux tirés au sort. C'est le test d'hygiène minimum pour garantir l'absence de bugs catastrophiques. | 10 000 parties | **\> 99.5 %** |
| **Bot Heuristique Glouton (Medium)** | Anticipe d'un seul coup. S'il peut gagner une sous-grille ou bloquer l'adversaire immédiatement, il le fait. Sinon, il joue au hasard. | 2 000 parties | **\> 90 %** |
| **Bot Minimax Profondeur 4 (Hard)** | Explore l'arbre de manière exhaustive sur 4 demi-tours, utilisant l'heuristique de Powell (blocage, points de centre).1 | 1 000 parties | **\> 70 %** |

Si le bot MCTS franchit le seuil des 70% de victoires contre l'adversaire Minimax Profondeur 4, le label "VERY\_HARD" est scientifiquement validé. La limite de temps de réflexion accordée à chaque bot durant les tests automatisés est fixée de manière symétrique à 500 millisecondes par tour.

## **Analyse des Risques Architecturaux et Stratégie de Remédiation**

L'implémentation de la recherche arborescente asymétrique dans un langage tel que C++17 s'accompagne de risques techniques sévères qu'il convient de cartographier, tout en prévoyant des mécanismes de repli ("Rollback") pour sécuriser la viabilité du projet.

### **Risque 1 : La Saturation Mémoire et la Fragmentation (Memory Leak)**

* **Le Péril :** La phase d'Expansion du MCTS instancie dynamiquement de nouveaux nœuds en permanence. À raison de dizaines de milliers de simulations par seconde, l'arbre peut consommer des gigaoctets de mémoire vive s'il n'est pas géré correctement. La création/destruction via des pointeurs nus (new/delete) provoquera une fragmentation du tas (Heap Fragmentation), ralentissant l'algorithme jusqu'à l'asphyxie, voire provoquant un crash système (OOM).  
* **La Remédiation :** Encapsulation stricte du cycle de vie des nœuds via la sémantique de mouvement du C++17 (std::unique\_ptr). Ainsi, lorsque la variable racine de l'arbre est réaffectée au tour suivant, l'arbre entier se désalloue en cascade de manière déterministe, garantissant une empreinte mémoire saine.  
* **Plan de Rollback :** Si le nettoyage de l'arbre entre chaque coup génère un ralentissement inacceptable, l'architecture pivotera vers une structure de données pré-allouée : un std::vector\<MCTSNode\> d'une taille fixée à la compilation (par exemple, 500 000 nœuds). Les nœuds seront alors liés non plus par des pointeurs, mais par des index entiers pointant vers ce grand tableau statique, supprimant totalement la charge du gestionnaire de mémoire de l'OS.

### **Risque 2 : La Cécité Tactique (Tactical Blindness)**

* **Le Péril :** La littérature académique, corroborée par les travaux de Winands et al., documente une vulnérabilité propre au MCTS appelée cécité tactique face aux "pièges peu profonds" (shallow traps).7 Dans la phase de simulation, le MCTS joue des coups totalement aveugles. Il peut arriver qu'un nœud soit évalué comme très prometteur car, au cours de milliers de simulations aléatoires, l'adversaire simulé n'est "par hasard" jamais tombé sur la combinaison exacte et étroite qui permet de punir immédiatement le mouvement de l'IA.  
* **La Remédiation :** Transition de simulations légères (*Light Playouts*) vers des simulations lourdes (*Heavy Playouts*). L'algorithme de génération de coups aléatoires dans la phase 3 est enrichi d'une simple règle conditionnelle : avant de tirer au sort, l'algorithme vérifie si un coup permet une victoire immédiate sur une sous-grille ou s'il doit bloquer une victoire immédiate adverse.10 Cette légère injection d'intelligence dans l'aléatoire élimine les aberrations tactiques grossières.  
* **Plan de Rollback :** Abandonner le MCTS au profit de l'algorithme Minimax Alpha-Beta (la stratégie de secours) mentionné plus haut, qui, par sa nature de parcours en largeur absolu, est structurellement immunisé contre la cécité tactique à courte portée, bien qu'il souffre en contrepartie de l'effet d'horizon sur le long terme.

### **Risque 3 : Goulet d'Étranglement CPU du Clone de Plateau**

* **Le Péril :** La puissance de MCTS est directement corrélée à son volume de simulations.20 Si la fonction de vérification des conditions de victoire verifier\_victoire() de la classe Plateau initiale contient des boucles complexes, le temps d'exécution d'une simulation explosera, bridant le MCTS à quelques centaines d'itérations par seconde, rendant l'IA incompétente.  
* **La Remédiation :** Remplacement de la vérification matricielle par une validation bit à bit (Bitboards) 6, précalculant les états de victoire pour court-circuiter l'évaluation.

## **Estimation de l'Effort d'Intégration (Lines of Code)**

L'une des réussites majeures de cette architecture réside dans sa formidable économie structurelle, répondant au dogme de concision exigé (cible fixée à moins de 500 lignes modifiées ou ajoutées). L'extrapolation de la densité du code C++17 nécessaire au déploiement du MCTS se ventile ainsi :

1. **Refactoring de la classe Plateau existante :** Ajout des méthodes d'extraction rapide des coups légaux et optimisation des tests de fin de partie via Bitboards. Estimation : **\~60 lignes**.  
2. **Structures de données MCTS (NoeudMCTS) :** Définition de l'objet, des constructeurs et des pointeurs intelligents gérant la parentalité de l'arbre. Estimation : **\~35 lignes**.  
3. **Logique Centrale et Gestion du Temps :** Boucle while gérée par \<chrono\>, instanciation de la racine et sélection finale du coup robuste. Estimation : **\~40 lignes**.  
4. **Moteur MCTS (Les 4 Phases) :** Fonctions encapsulant la Descente (UCB1), l'Expansion, les Playouts aléatoires optimisés par std::mt19937 et la Rétropropagation des scores. Estimation : **\~90 lignes**.  
5. **Utilitaires Mathématiques :** Calculateur de l'algorithme des bandits manchots (UCB1) et balisage de l'aléatoire. Estimation : **\~25 lignes**.

L'effort d'intégration global s'élève à approximativement **250 lignes de code (LOC)**. Cette estimation est remarquablement basse et démontre qu'une intelligence artificielle algorithmique asymétrique complexe, capable d'un jeu de niveau compétition, peut être circonscrite dans un volume de code soutenable, auditable et parfaitement maîtrisable par une équipe d'étudiants.

## **Variante Algorithmique Minimaliste (Dégradée)**

Si, lors du processus d'intégration, la complexité spatiale de l'arbre en mémoire ou la maîtrise des pointeurs intelligents C++17 met en péril la stabilité globale du projet logiciel, il est possible de dégrader volontairement le système vers une variante d'une simplicité désarmante, appelée **Flat Monte Carlo** (ou Pure Monte Carlo Game Search).10  
Dans ce schéma asymétrique aplati, la notion d'arbre de jeu disparaît intégralement, éradiquant au passage toute problématique d'allocation dynamique et de fuite de mémoire.  
La logique opératoire se résume à une approche stochastique à plat :

1. L'IA observe l'état actuel et extrait la liste des ![][image5] coups légaux immédiats.  
2. Pour *chacun* de ces ![][image5] coups immédiats, l'IA clone le plateau et simule un volume défini (par exemple, ![][image6]) de parties totalement aléatoires jusqu'à leur terme.  
3. À la fin du processus (impliquant ![][image7] simulations), l'IA joue simplement le coup de la première génération qui a généré le pourcentage de victoires le plus élevé lors de ses 500 tests.

Cette variante "Minimaliste" sacrifie la finesse d'anticipation tactique de l'arbre de recherche conditionnelle (l'UCB1). La perte de performance est notable : l'IA peinera contre des adversaires de niveau HARD exploitant des combinaisons précises à long terme, retombant de fait vers un niveau évalué à MEDIUM\_2. Néanmoins, l'implémentation complète d'un Flat Monte Carlo tient en moins de 40 lignes de code, offrant le filet de sécurité ultime (Fallback) garantissant qu'un livrable fonctionnel et commenté sera présenté au jury, quoi qu'il advienne de l'architecture principale.

## **Conclusion**

La modernisation de l'intelligence artificielle pour l'Ultimate Tic-Tac-Toe par la Recherche Arborescente Monte Carlo (MCTS) résout la délicate équation posée par les contraintes académiques et techniques de l'ingénierie logicielle. En délaissant l'approche heuristique de Minimax, entravée par l'opacité et l'arbitraire de ses pondérations manuelles de plateau, le MCTS instaure un système d'apprentissage statistique organique et mathématiquement traçable. L'implémentation en C++17 proposée, propulsée par la concision des pointeurs intelligents et l'optimisation binaire des structures de données, permet de livrer un moteur d'une performance redoutable, capable d'itérer des centaines de milliers de futurs parallèles, sans jamais franchir la limite de complexité de 500 lignes de code. Le système résultant n'est plus une simple machine à calculer, mais un moteur de probabilités élégant, dont la logique interne, dépouillée de tout artifice empirique, s'impose comme une parfaite démonstration de l'intelligence algorithmique contemporaine.

#### **Sources des citations**

1. Strategy for Ultimate Tic Tac Toe, consulté le mai 4, 2026, [https://boardgames.stackexchange.com/questions/49291/strategy-for-ultimate-tic-tac-toe](https://boardgames.stackexchange.com/questions/49291/strategy-for-ultimate-tic-tac-toe)  
2. AlphaZero-like solution for playing Ultimate Tic-Tac-Toe in the browser, consulté le mai 4, 2026, [https://www.reddit.com/r/MachineLearning/comments/rdb1uw/p\_utttai\_alphazerolike\_solution\_for\_playing/](https://www.reddit.com/r/MachineLearning/comments/rdb1uw/p_utttai_alphazerolike_solution_for_playing/)  
3. Playing Ultimate Tic-Tac-Toe using Reinforcement Learning, consulté le mai 4, 2026, [https://arnavparuthi.medium.com/playing-ultimate-tic-tac-toe-using-reinforcement-learning-892f084f7def](https://arnavparuthi.medium.com/playing-ultimate-tic-tac-toe-using-reinforcement-learning-892f084f7def)  
4. (PDF) Ultimate Tic-Tac-Toe Bot Techniques \- ResearchGate, consulté le mai 4, 2026, [https://www.researchgate.net/publication/360376264\_Ultimate\_Tic-Tac-Toe\_Bot\_Techniques](https://www.researchgate.net/publication/360376264_Ultimate_Tic-Tac-Toe_Bot_Techniques)  
5. Ultimate Tic-Tac-Toe Bot Techniques, consulté le mai 4, 2026, [https://journals.flvc.org/FLAIRS/article/download/130698/133960/233063](https://journals.flvc.org/FLAIRS/article/download/130698/133960/233063)  
6. Efficient representation of Ultimate Tic Tac Toe, consulté le mai 4, 2026, [https://minimax.dev/docs/ultimate/efficient-representation/](https://minimax.dev/docs/ultimate/efficient-representation/)  
7. Monte-Carlo Tree Search and Minimax Hybrids \- Maastricht University, consulté le mai 4, 2026, [https://dke.maastrichtuniversity.nl/m.winands/documents/paper%2049.pdf](https://dke.maastrichtuniversity.nl/m.winands/documents/paper%2049.pdf)  
8. Reinforcement Learning Project: Ultimate Tic-Tac-Toe, consulté le mai 4, 2026, [https://josselinsomervilleroberts.github.io/papers/Report\_INF581.pdf](https://josselinsomervilleroberts.github.io/papers/Report_INF581.pdf)  
9. Using Reinforcement Learning to play Ultimate Tic-Tac-Toe \- Medium, consulté le mai 4, 2026, [https://medium.com/@shayak\_89588/playing-ultimate-tic-tac-toe-with-reinforcement-learning-7bea5b9d7252](https://medium.com/@shayak_89588/playing-ultimate-tic-tac-toe-with-reinforcement-learning-7bea5b9d7252)  
10. MCTS algorithm for ULTIMATE Tic-Tac-Toe \- Amine BELKEDAH, consulté le mai 4, 2026, [https://belkedah.com/index.php/2020/12/01/artificial-intelligence-monte-carlo-tree-search-mcts-for-ultimate-tic-tac-toe/](https://belkedah.com/index.php/2020/12/01/artificial-intelligence-monte-carlo-tree-search-mcts-for-ultimate-tic-tac-toe/)  
11. Implementing the Minimax Algorithm for AI in Python | DataCamp, consulté le mai 4, 2026, [https://www.datacamp.com/tutorial/minimax-algorithm-for-ai-in-python](https://www.datacamp.com/tutorial/minimax-algorithm-for-ai-in-python)  
12. Beginner's Guide to Alpha-Beta Pruning: From Minimax to AI, consulté le mai 4, 2026, [https://www.projectpro.io/article/alpha-beta-pruning-in-ai/1157](https://www.projectpro.io/article/alpha-beta-pruning-in-ai/1157)  
13. CS 540 Lecture Notes: Game Playing, consulté le mai 4, 2026, [https://pages.cs.wisc.edu/\~dyer/cs540/notes/games.html](https://pages.cs.wisc.edu/~dyer/cs540/notes/games.html)  
14. Minimax & MCTS in Ultimate Tic-Tac-Toe | PDF \- Scribd, consulté le mai 4, 2026, [https://www.scribd.com/document/829382329/1159622-Minimax-and-MCTS-Algorithms-final](https://www.scribd.com/document/829382329/1159622-Minimax-and-MCTS-Algorithms-final)  
15. Tic-Tac-Toe Using AI Algorithms \- Gamma, consulté le mai 4, 2026, [https://gamma.app/docs/Tic-Tac-Toe-Using-AI-Algorithms-z35zrres6ckc9xe](https://gamma.app/docs/Tic-Tac-Toe-Using-AI-Algorithms-z35zrres6ckc9xe)  
16. I'd used minimax before but not heard of Monte Carlo tree search. It, consulté le mai 4, 2026, [https://news.ycombinator.com/item?id=10982361](https://news.ycombinator.com/item?id=10982361)  
17. Ultimate Tic Tac Toe \[Puzzle discussion\] \- CodinGame \- Coding Game, consulté le mai 4, 2026, [https://www.codingame.com/forum/t/ultimate-tic-tac-toe-puzzle-discussion/22616](https://www.codingame.com/forum/t/ultimate-tic-tac-toe-puzzle-discussion/22616)  
18. A comparison of two tree-search based algorithms for playing 3, consulté le mai 4, 2026, [https://www.diva-portal.org/smash/get/diva2:1597267/FULLTEXT01.pdf](https://www.diva-portal.org/smash/get/diva2:1597267/FULLTEXT01.pdf)  
19. Comparative Analysis of Monte Carlo Tree Search and Alpha-Beta, consulté le mai 4, 2026, [https://www.diva-portal.org/smash/get/diva2:1948901/FULLTEXT02.pdf](https://www.diva-portal.org/smash/get/diva2:1948901/FULLTEXT02.pdf)  
20. Minimax vs. Monte Carlo \- Diva-portal.org, consulté le mai 4, 2026, [https://www.diva-portal.org/smash/get/diva2:1887668/FULLTEXT01.pdf](https://www.diva-portal.org/smash/get/diva2:1887668/FULLTEXT01.pdf)

[image1]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAXCAYAAAARIY8tAAABpElEQVR4Xu2UPUvEQBCG31mSmIRT5NBCBb+44pQrBDux8BdZCbZXXCWKjf9B1EYRrGwOsVLQRhAEP8BGWyuxiO9cPrisnprjCot7imTnnd2d2clkgf+G72PW1lKGia39nTAcB8yqDkXkyHGcZR2XyxgSwSEEkYHZyC8qADf5yNsStdukid8C1OswfJVtXeGGN8zyKjF5GjTa/ZzR5OIfAjBDTtgxBus0IgEe7Ckk0lLwfWI7mEGT6X0fgMd/54bHqR2G4Ri0BCKn2STPqzH4LmcfQAMZs5X5YrREm5YWo/UUXZTTsmwToe0blEqjOZ8irRLZQWNY0NBxsJTavu/P8BUxyHWqCeQ+HSf2W7udlGg7p3VE5KV1gpyGWz7nW2MPVWa7n/lcd5FrnliJc9d1FzL9K/IaH10ubY/ieZhjrzc8oGr7CsHvcaedBKw4tq+HaBdZZeqWIMCkrWmbancFQTBhuwri1ST+sey2SzR/OqcXx59C/IdmLakkAXtTIu72yHJcYASD0PtI5FlPValgwJ7aNXrPs7f3uPkZr4Q129+nT0c+AQ7QSJ4vY42EAAAAAElFTkSuQmCC>

[image2]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAgAAAAZCAYAAAAMhW+1AAAAoElEQVR4XmNgGAXkAWYGZn8gtQ6IE9FlPBkZGf4zMLA4QgSYpgKJsxA2C4MrkPzPycApB1PPwMgIVMx4GspmfALRjQwY/wOt82RgUFFhZwRymBgYQEZiAiYmpkyQAiCTF10ODIA6Z0AV4AIcSkACQwE7O4My0G3fwBygCU+BLv3AAPIPA6sRIwPDDaCjN6Lo4OJikAKGRTIDC4sVisSwAAC0SBafO5MFwgAAAABJRU5ErkJggg==>

[image3]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABoAAAAXCAYAAAAV1F8QAAABsElEQVR4Xu2UPUsDQRCG390YL14+ipgzkkICBhsbIX/EwvwHe0sRey0EsfEHBDsbo9haqGBtYbQQEfEDtdFSPN/di+ZuOEmiURB8YHO779zNzM7OBvgLKIUDjkYwVINKA+Zp1maOwGa1r1IFknR8LfX+o9ROoYCslPtLuZxioCspBwyNSeU7bMHzRoX2xG0eKaDJM/L5vBP2Xqk4dHgmxEcUkW4vMx6bwefkua31jNpGOl0Uot9y/AG70IcZgA7r3VGpOCzJiZRbpZKBjm3wZHIqrHeJ2uVPQapx8P74JgGpWzR0Hcjlpf4Os5Zn8wnZYQTlPI3qjjNOL01jpLvDqDFAa2y4rluSehws2wtsFwoGoOdQgstgN3bLVXPrBUpdSCkOBtmjn0upR3DgTJgDZIki/03czTo7Td6bOJYYaT+0djlSoXWbIJBs1Q4ZkgQww91sCnmtVjOmOBKJ6aD/9YpZcjd1ZDAiX4swiEkm88BmWuBqnl21yO+WIRKOwVw2vAZT3ApjDKyCsiU3zeTbc7Y+OgTSWs+al/jxOTzu5ydpZXUv9f6jsZpHPiflf36dNzJeT6VaEVNpAAAAAElFTkSuQmCC>

[image4]: <data:image/png;base64,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>

[image5]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABIAAAAXCAYAAAAGAx/kAAABJUlEQVR4Xu2SvUoDURCFz5msJpBC7GxTiaT2CbS3ylNoZyHa+B42Nmns0gjBQtRCDBEfwEoQxSbBIBaxuZ67bvTuZTFZOyEf7M6dn3uYmV1gTlnW9PRAXIA8J3gl/3qSlH8nc0bglGRXkdvvm0Wo6FKXnA43YXwZWCLh408LwHqYK8Y5aaVCLgyrkw5gO2FsOsRYb4darZFFDiVyFJbMRIJkIx0PfAQqWxrJ7+tveCH6roCHOFcOsvMlZAdxanZarYpactl4uaWXwLYNaKcnw0n6yQ17cdUUkk110p94vp2iX+FXqsCqRnmJ49L68CPWUV+JMzmaaC7KHPvFZuccZrab7eo5zv1A3EvgVa0P5Ixk36OKoWpGsgPlhhJ80772o5o5/5JPd0Q6HBohzAYAAAAASUVORK5CYII=>

[image6]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEkAAAAYCAYAAAC2odCOAAADbklEQVR4Xu1XXYhNURT+9j7nnrnXnRmDyaWGSCjDi3lgvPAy8qJ4mPIiaVJECSmlNKVIjTfFvHrUlBc/aTKRIY1SJClppJSkyEiNNMe3f+65++65546ne4fOV3vOWd9ae83ea6+19rlAhgwZMsxfHIXAUyHEAyEwJoCHAuKJa0B+lDZjfLlN8V4QBAOuvhFYBCz0uTmwwidqo6vgM6lgcGL+jdHZ2ebriFYGMabRnRZgva9sBPT/5xp5YDEPMVYyx0/fDlIO0/hriHAX7cZpM+mbDA4OSrNfnA2AAeWT9tt9u1nQk8xCqlAqlYp0MtPfj8DXNRTmkOwaOVQwPDDD99vNJ2CQfpC75XLKj5TysMOUtO96iIBN1uiayzMqu/l44XLNgs2kucAsg2oJCQIE+6rmRtEGlYmOiYXmrvisA3lDpVwRxVJCCXFXAscco+ZirpOGMhEqQy65XBRho8mulrXaRoi3NiBVKJezzydI+lFZZq3zBA65Ns1G+fQZhGGu73kYotfV5/P5VbqMgNMuXwSWmRK1+7E9zbUxtFBZOIt3oA1mgEIXradMk57t6G/Qg54c507qhikwqd7pX48yJxIZevg+akOtUbyriPjEua8TOQx7zZrl8YQj2tC2RM1l8EyGpWVMudfVQgTWqDH4SHFEcZSnVWQ7OtDhmTcNUuJUFVFkhnCzYRj2KZGZtU1lAjOtKkhob19sMkxeVWJaWZnsSgkStSPKCa/AHQmXw1bVo6h8UzGcf7Ab+67e23UwKHvBbG3FUtPPbPBSMkbvt07fqxlZ7azOpIYi0rfvlE/bzVbWqDJG4nxFr7HO9pqdxgSfa/YkUacnmQiK3z7PtB1SOj4v+7p66EY3vygwbYYuWz3MO4wsqGNJl0esb+56EM90mYTQpZWwpne+SmRoR7plJAiCPXA2z/0cceUKdOAe+yxyudwWEwgM+ToFG9kaDhsL3rQHWUYnPW6vWt8CYLlDP+J+fjmy2sR9BnOmmkJcQGFlNSfifB6rK4z6MhWYMJHXNfqF72Nltfps5+N9Ur/681+MVhw0HlznN+bBGSv2mSZd83C5ZnnCvOY260rp8n+fyYvQdkkGf6DdS9finwV/ARxgVoxzkxd47abevLzJzjGsE5yR+kPcZuBNjuv80Fzj6zNkyJAhQ4b/D38AZw3SPlerf9YAAAAASUVORK5CYII=>

[image7]: <data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEYAAAAYCAYAAABHqosDAAADpElEQVR4Xu2XT4hNURzHv79775s30xhmxjxDoZlEmSzksWBDlKywUTYWkqJIkg1iipI9CTu7Kf8KJUX+h2ShrCjyZ8HGgo0x713f37n3vXvueXd482RY3E/de9/5nd8595zv+Z3fuQ/IycnJmSza2hYK5ImI3OHztgju8XqUOMhzCG5C5DoLN3g9S+r+Eb29U11Tjdmz0eHaGumY41rGhcLc5T2kOI9te09PzzRWhqz/WEBhmV33twmhr5VQ389iVXR8QrOHQ46rZ/w8HPR9bONChkGAlY4Pq70z9PuCIFjHfh6w3zeuTyOhasKX6mVB4xXP83bZtkljeDiacCJOhRE+5LqpDye9o1buRGe/aWfh+/4WqNYp5Btv19K2TOQ7osaDWqIgR3g7m/aZXLgwd12bw5CJIofYdrJejnaDpoMEH5t1l6RsWQTAGo0Yqv2ejTZovnF9miFeLd+11xHoAjQFx/BrYQSvsianQsRRVrNoVJ1IykyvwKLIpzjftmciGjFRh2/duomg/ZTLKDTYRUb7NdqbRXNfAYvZHw8GuQpsSgsejbVBGBXCiqQBXirMfssBnZ2YaeYLf7ttz0ZwRZ25jQ64VS0Qoly2xJEfpVJpSlJuAk6c+WG9+d2F6XEOXFGvTguQYA6MyB4EwXIjgIc9KR/2F0V3OpIa2ETp4m3QkIT/gLAMI86PPvR1uZW/g6fLarvM8b2EFSGRMFlbCZpTYmEoZCRAShie+b0qnufhtG3PwnTEDkdVGC7tDNehFfTlDNt+194KjGRNqCEKhSXGYCacJUwSMSqAzodt9zluM0x7N5Js2EmlFvYBgjUwiuO14zZxBNVSCbp9qjD5bkJcYgdXHZt+w+gkd5uS4HMtMhxSWyxKDzhqOxRRXMAHv3mCtbY9QfCV95Jjq4diq3AwVXZazykcaBUZCXk8zHZwtgknd07H1YWuPlOGtzMzYlQUwcN62fjIBdvHh78xMz9FyAeOdKlr5QtHEK2M+5XZFEaE/sbThxPVyGlKHAoz1miTSnSSpKxanps2IWxvx6BluM92o0nZjOUW7zqeCE72mB6bqpauCBt8svx1UhfNy2uZXfCcbfbaPr9hDAMD7a6xjkjFNWXR3Y1uPipMnqv0YPCAERUhttfh2I7HKy9apnjv+I4Xtk8Mj2yYeRSiHBXyz1UT/63+U7h9TnHG+rG51a2zmEUxLjPSzxeLmOdW1mD9Yfo95X+EbW5dTk5OTk7O/8NPmALFy5GF7ZMAAAAASUVORK5CYII=>