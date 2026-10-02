# Maze Generation — Growing Tree Algorithm

A C project developed in the GM3 Applied Mathematics programme at INSA Rouen Normandie. It generates random perfect mazes with four Growing Tree cell-selection strategies and provides an interactive terminal game using ncurses.

## Authors and task allocation

**Manh Hung Nguyen** and **Tan Minh Duy Ngo** — GM3, INSA Rouen Normandie.

The following task allocation is proposed around two complementary modules:

| Team member | Main responsibilities | Main files |
| --- | --- | --- |
| **Tan Minh Duy Ngo** | Maze data structures; Growing Tree generation loop; active-list memory management; four cell-selection strategies; neighbour detection and wall removal. | `src/growingTree.c`, `src/fonctions.c`, `include/types.h`, `include/fonctions.h` |
| **Manh Hung Nguyen** | ncurses interface; maze rendering in game mode; keyboard navigation and collision checks; path marking and backtracking visualisation; victory detection; build configuration. | `src/game_ncurses.c`, `include/game_ncurses.h`, `Makefile` |
| **Both** | Algorithm design discussions, integration between generator and game, manual validation of generated mazes and gameplay, project documentation and presentation. | Shared project work |

## Features

- Configurable grid dimensions, from 1 to 100 rows and columns.
- User-selected starting cell, with zero-based coordinates.
- Four strategies for choosing the next active cell.
- ASCII maze display.
- Interactive navigation with wall collision checks, path marking and victory detection.

## Algorithm

The grid starts with all walls closed. The Growing Tree algorithm maintains a dynamic list of active cells:

1. Mark the starting cell as visited and add it to the list.
2. Select an active cell using the chosen strategy.
3. If it has unvisited neighbours, choose one randomly, remove the shared wall, mark the neighbour as visited and add it to the list.
4. Otherwise, remove the selected cell from the list.
5. Repeat until the list is empty.

Each new cell is connected to the existing maze through a single passage. Under normal execution, the resulting cell graph is a spanning tree: it is connected, has no cycles and contains exactly one simple path between any two cells.

### Selection strategies

| Menu | Function | Behaviour |
| --- | --- | --- |
| 1 | `theMRC` | Select the most recently added active cell; depth-first backtracking behaviour, typically producing long corridors. |
| 2 | `randomList` | Select an active cell uniformly at random. |
| 3 | `lastNList` | Select randomly among the last N active cells. In this implementation, if N exceeds the current list size, the window becomes half that size, with a minimum of one cell. |
| 4 | `lastORrand` | Mix most-recent and random selection using integer weights `pLast` and `pRand`. |

For nonnegative weights with a positive sum, strategy 4 selects the most-recent branch with probability `pLast / (pLast + pRand)`. The current menu requires `pLast > pRand + 2`. For example, use `pRand = 1` and `pLast = 4`.

## Build and run

### Requirements

- A C11 compiler, such as GCC.
- GNU Make.
- ncurses development headers and library.
- An interactive terminal for game mode.

On Ubuntu or Debian:

```bash
sudo apt update
sudo apt install build-essential libncurses-dev
```

### Clone and compile

```bash
git clone https://github.com/manhhgdnh/maze-generation-growing-tree-GM3.git
cd maze-generation-growing-tree-GM3
make
./maze
```

You can also compile and launch the program with `make run`.

Direct compilation is also possible:

```bash
gcc -Wall -Wextra -std=c11 -Iinclude \
    src/growingTree.c src/fonctions.c src/game_ncurses.c \
    -o maze -lncurses
```

To remove generated object files and the executable, use `make clean`.

## Usage

The program prompts for:

1. Number of rows and columns.
2. Selection strategy, from 1 to 4.
3. Strategy parameters, where applicable.
4. Starting row and column, indexed from zero.
5. Display mode: `1` for ASCII output or `2` for the ncurses game.

For a first run, choose a **10 × 15** grid, strategy **1**, starting cell **(0, 0)** and mode **2**.

### Game controls

The player starts at the cell selected before generation. The goal is the bottom-right cell, at `(rows - 1, cols - 1)`.

| Key / symbol | Meaning |
| --- | --- |
| Arrow keys | Move through open passages. |
| `x` or `X` | Enable path marking; forward moves leave marks and immediate backtracking removes them. |
| `n` or `N` | Disable path marking; existing marks elsewhere remain. |
| `q` | Quit, including after victory. |
| `D` | Player position. |
| `G` | Goal cell. |
| `X` | Marked path cell. |

Choose a starting cell different from the goal. The current game checks victory after a successful move, rather than at initialisation.

## Repository structure

| Path | Role |
| --- | --- |
| `src/growingTree.c` | Entry point, input prompts and maze-generation loop. |
| `src/fonctions.c` | Grid operations, wall removal, dynamic active list and selection strategies. |
| `src/game_ncurses.c` | Terminal rendering, player movement, marking and victory detection. |
| `include/types.h` | Cell, maze, active-list and direction definitions. |
| `include/fonctions.h` | Maze-generation helper declarations. |
| `include/game_ncurses.h` | Game entry-point declaration. |
| `Makefile` | Compilation, run and clean targets. |
| `Labyrinthe.pdf` | Project document. |
| `README.md` | Project overview and usage instructions. |

## Implementation notes and limitations

- The maze uses a fixed-capacity `100 × 100` array; the active-cell list grows dynamically through `malloc` and `realloc`.
- Random generation is seeded with the current time. There is no user-configurable seed for reproducible runs.
- Input handling assumes valid integer input. Use positive N for strategy 3 and nonnegative weights for strategy 4; these conditions are not fully validated by the menu.
- Game mode has no scrolling or terminal-size validation. The maze drawing occupies `2 × rows + 1` lines and `4 × cols + 1` columns, with additional space required below for instructions and messages.
- The goal is an internal cell; the program does not open an exit through the outer boundary.
- There is no automatic maze solver or export feature.
- Active-cell lookup and removal are linear in the list size, so a linear-time bound for the full generator should not be assumed.

## Academic context

This project demonstrates procedural generation, graph traversal, dynamic memory management, modular C programming and terminal interaction with ncurses.
