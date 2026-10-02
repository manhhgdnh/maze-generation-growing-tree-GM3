
/*
Implementation of the main functions for the Maze project.

Content :
    - Maze initialization (initMaze)
    - Maze drawing (drawMaze)
    - Dynamic list management (used in Growing Tree algorithm)
    - Utility functions: neighbor checking, wall deletion, direction choice, mouvement
    - Different cell selection strategies (most recent, random, last-n, last or random)

Notes :
    - Maze is assumed to be a 2D array of Cell (see type.h).
    - Direction NORTH, EAST, SOUTH, WEST, and NO_DIR are defined in Direction unumerate.
    - The ListCell structure stores pointers to existing Cell objects in the maze grid
*/


#include <stdio.h>
#include "types.h"
#include "fonctions.h"
#include <stdbool.h>
#include <stdlib.h>

/*
Initializes all cells in the maze : 
    - Marks them as unvisited
    - Sets their (x,y) coordinates
    - Closes all walls by default
*/

void initMaze(Maze m, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m[i][j].visited = false;
            m[i][j].x = j;
            m[i][j].y = i;
            for (int k = 0; k < 4; k++) {
                m[i][j].walls[k] = true;
            }
        }
    }
}

/*
Draw maze in ASCII art.
    - '+' are corners
    - '---' are horizontal walls
    - '|' are vertical walls
    - Spaces represent no wall
*/

void drawMaze(Maze m, int rows, int cols) {
    // Draw top border 
    for (int col = 0; col < cols; col++) {
        printf ("+");
        if (m[0][col].walls[NORTH]) printf("---");
        else                        printf("   ");
    }
    printf("+\n");

    // Draw sides and bottom lines row by row
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++){
            if (col == 0) {
                // Left edge of the maze
                if (m[row][col].walls[WEST]) printf("|");
                else                         printf(" ");
            
            }

            printf("   "); // Inside of the cell

            // Right wall or passage
            if (m[row][col].walls[EAST]) printf("|");
            else                         printf(" ");

        }
        printf("\n");

        // Draw the bottom line for this row
        for (int col = 0; col < cols; col++) {
            printf("+");
            if (m[row][col].walls[SOUTH]) printf("---");
            else                          printf("   ");
        }
        printf("+\n");
    }
}

/*
Initializes a dynamic list of Cell pointers used by the Growing Tree algorithm.
Starts with a small capacity that grows as cells are added.
*/

void initListCell(ListCell *list) {
    list->nbCell = 0;
    list->capacity = 3;
    list->cells = (Cell**)malloc(list->capacity * sizeof(Cell*));
    if (list->cells == NULL) {
        list->capacity = 0;
    }
}   

/*
Frees the memory used by the list of Cells.
*/

void freeListCell(ListCell *list) {
    free(list->cells);
    list->cells = NULL;
    list->nbCell = 0;
    list->capacity = 0;
}

/*
Add a new cell pointer to the list.
The list grows dynamically, doubles its capacity if full.
*/

void saveToList(ListCell *list, Cell *c) {
    if (list->nbCell == list->capacity) {
        list->capacity *= 2;
        Cell **temp = (Cell**)realloc(list->cells, list->capacity * sizeof(Cell*));
        if (temp == NULL) {
            return;
        }
        list->cells = temp;
    }
    list->cells[list->nbCell] = c;
    list->nbCell++;
}

/*
Remove a cell pointer from the list at the specified index.
Shifts all subsequent elements left by one position.
*/

void removeFromList(ListCell *list, int index) {
    for (int i = index; i < list->nbCell - 1; i++) {
        list->cells[i] = list->cells[i+1]; 
    }
    list->nbCell--;
}
 
/*
Check all four neighboring cells of a given cell.
Returns an ArrayOfBool where true means the neighbor exists and has not been visited yet.
*/

ArrayOfBool checkNeighbours(Maze m, int rows, int cols, const Cell *c) {
    ArrayOfBool res = { {false, false, false, false} };

    int row = c->y;
    int col = c->x;
    // Check North
    if (row > 0 && m[row - 1][col].visited == false) {
        res.data[NORTH] = true; 
    } 
    // Check East
    if (col + 1 < cols && m[row][col + 1].visited == false) {
        res.data[EAST] = true;
    }
    // Check South
    if (row + 1 < rows && m[row + 1][col].visited == false) {
        res.data[SOUTH] = true;
    }
    // Check West
    if (col > 0 && m[row][col - 1].visited == false) {
        res.data[WEST] = true;
    } 
    
    return res;
}

/*
Removes the wall between two adjacent cells in the specified direction.
Updates both cells to maintain consistency.
*/

void deleteWalls(Cell *c1, Cell *c2, Direction dir) {
    switch (dir) {
    case NORTH:
        c1->walls[NORTH] = false;
        c2->walls[SOUTH] = false;
        break;
    case EAST:
        c1->walls[EAST] = false;
        c2->walls[WEST] = false;
        break;
    case SOUTH:
        c1->walls[SOUTH] = false;
        c2->walls[NORTH] = false;
        break;
    case WEST:
        c1->walls[WEST] = false;
        c2->walls[EAST] = false;
        break;
    default:
        break;
    }
}

/*
Choose a random direction among available neighbors.
Returns NO_DIR if no valid neighbor is available.
*/

Direction chooseDirection(ArrayOfBool neighbours) {
    int dir[4];
    int count = 0;
    for (int i = 0; i < 4; i++) {
        if (neighbours.data[i] == true) {
            dir[count++] = i;
        }
    }
    if (count == 0) return NO_DIR;
    int random = rand() % count;
    return (Direction)dir[random];
}

/*
Return a pointer to the neighboring cell in the given direction.
Return NULL if the direction leads outside the maze boundaries.
*/

Cell* moveToCell(Maze m, int rows, int cols, const Cell *c, Direction dir) {
    if (!c) return NULL;
    
    int row = c->y;
    int col = c->x;

    switch (dir) {
    case NORTH:
        if (row > 0) return &m[row - 1][col];
        break;
    case EAST:
        if (col + 1 < cols) return &m[row][col + 1];
        break;
    case SOUTH:
        if (row + 1 < rows) return &m[row + 1][col];
        break;
    case WEST:
        if (col > 0) return &m[row][col - 1];
        break;
    default:
        return NULL;
    }
    return NULL;
}

/*
Return the most recent cell (MRC) from the list - the last one added.
*/

Cell* theMRC(ListCell *list) {
    if (!list || list->nbCell <= 0) return NULL;
    return list->cells[list->nbCell - 1];
}

/*
Same as theMRC() but returns a boolean success.
Useful when you need to check for validity.
*/

bool theMRC_val(const ListCell *list, Cell **out) {
    if (!list || list->nbCell <= 0 || !out) return false;
    *out = list->cells[list->nbCell - 1];
    return true;
}

/*
Selects and returns a random cell from the list
*/

Cell* randomList(ListCell *list) {
    if (!list || list->nbCell <= 0) return NULL;

    int random = rand() % list->nbCell;    
    return list->cells[random];
}

/*
Selects a randim cell among the last N added cells.
If N > list size , uses half the current list size instead.
*/

Cell* lastNList(ListCell *list, int N) {
    if (!list || list->nbCell <= 0) return NULL;

    int k = N;
    if (k > list->nbCell) k = (int)(0.5 * list->nbCell);
    if (k <= 0) k = 1;

    int start = list->nbCell - k;
    int index = start + (rand() % k);
    return list->cells[index];
}

/*
Randomly selects between:
    - the most recent cell (with probability pLast)
    - a random cell (with probability pRand)
*/

Cell* lastORrand(ListCell *list, int pLast, int pRand) {
    if (!list || list->nbCell <= 0) return NULL;

    int total = pLast + pRand;
    int proba = rand() % total;

    if (proba < pLast) {
        return theMRC(list);
    } else { 
        return randomList(list);
    }
}