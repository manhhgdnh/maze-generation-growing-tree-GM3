#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
 
#define MAX 100

typedef struct {
    int x;    
    int y;
    bool visited;
    bool walls[4];
} Cell;

typedef Cell Maze[MAX][MAX];

typedef struct {
    Cell **cells;
    int nbCell;
    int capacity;
} ListCell;

typedef struct {
    bool data[4];
} ArrayOfBool;

typedef enum { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3, NO_DIR = -1 } Direction;

#endif