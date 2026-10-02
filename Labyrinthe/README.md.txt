# Labyrinth Project – Growing Tree Algorithm

## Author
NGUYEN Manh Hung, NGO Tan Minh Duy 
INSA Rouen  
Date: October 2025  

---

## Description
This project generates random *perfect mazes* using the **Growing Tree algorithm**.

The algorithm starts with a grid of isolated cells and progressively removes walls between them to form a single connected maze with no loops.

You can choose between several *cell selection strategies* that affect the maze’s shape:

| Method | Description | Maze characteristics |
|---------|--------------|----------------------|
| **theMRC** | Always pick the most recent cell (DFS style) | River-like, short paths |
| **randomList** | Pick a random active cell | More random structure |
| **lastNList** | Pick randomly among the last *N* added cells | Medium complexity |
| **lastORrand** | Mix between “most recent” and “random” | Balanced randomness |

After generation, you can:
- View the maze in ASCII
- Play interactively using **ncurses** (move with arrow keys and reach the goal)

---

## Build and Run

### Requirements
- **GCC compiler**
- **ncurses** library  
  (Install on Ubuntu/Debian with `sudo apt install libncurses5-dev`)

### Build
```bash
make
