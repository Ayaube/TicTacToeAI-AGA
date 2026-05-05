# Diagramme de classe — TicTacToeAI-AGA

```mermaid
classDiagram
    direction TB

    class IGame {
        <<interface>>
        +getWinner() Winner
        +isFinish() bool
        +isAllGameFinish() bool
        +getMove(outMove : GameMove&) bool
        +setMove(move : GameMove) void
        +initialize(nbGame : uint, level : Level, mode : Mode, alwaysPlayFirst : bool, alias : string) void
    }

    class Plateau {
        -m_g : int[9][9]
        -m_e : int[3][3]
        -jouerCoup(row : int, col : int, joueur : int) int
        -annulerCoup(row : int, col : int, ancienEtat : int) void
        -etatSousPlateau(si : int, sj : int) int
        -getCoupsLegauxFast(last : GameMove, buf : GameMove[]) int
        -minimax(last : GameMove, depth : int, alpha : int, beta : int, joueur : int) int
        -evaluer() int
        -urgenceMetaGrille(joueur : int) int
        +Plateau()
        +getCase(row : int, col : int) int
        +setCase(row : int, col : int, val : int) void
        +getEtat() int[3][3]
        +affiche_plateau() void
        +gagnant(dL : int, dC : int) int
        +verifPlateau() void
        +estCondamne(row : int, col : int) bool
        +estPlein(si : int, sj : int) bool
        +gagnantMetaGrille() int
        +getCoupsLegaux(last : GameMove) vector~GameMove~
        +prochainMove(myMove : GameMove&, lastMove : GameMove&) void
    }

    class GameMove {
        <<struct>>
        +row : int
        +col : int
    }

    class Level {
        <<enumeration>>
        EASY_1
        EASY_2
        MEDIUM_1
        MEDIUM_2
        HARD_1
        HARD_2
        VERY_HARD_1
        VERY_HARD_2
    }

    class Mode {
        <<enumeration>>
        DEBUG
        ARENA
    }

    class Winner {
        <<enumeration>>
        NO_WINNER
        IA
        PLAYER
        IA_AND_PLAYER
    }

    Plateau ..> GameMove : utilise
    IGame ..> GameMove : utilise
    IGame ..> Level : prend en paramètre
    IGame ..> Mode : prend en paramètre
    IGame ..> Winner : retourne
```

## Légende des relations

| Symbole | Signification |
|---|---|
| `..>` | Dépendance (utilise dans une signature) |
| `<<interface>>` | Classe abstraite pure — ne peut pas être instanciée |
| `<<struct>>` | Structure de données simple |
| `<<enumeration>>` | Énumération |
| `-` | Membre privé |
| `+` | Membre public |

## Notes

- `IGame` est fournie par le prof via `libUTTTLib.a`. Notre code n'en voit que l'interface.
- `Plateau` est la seule classe que nous avons écrite. Elle contient **toute** la logique IA.
- `GameMove` est le type d'échange universel : notre IA lit le coup adverse (`getMove`) et envoie le sien (`setMove`) via ce struct.
- La variable globale `extern IGame& game` est définie dans `libUTTTLib.a` et utilisée dans `main.cpp`.
