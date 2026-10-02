#ifndef FONCTIONS_H
#define FONCTIONS_H

#include "types.h"
 
void initMaze(Maze m, int rows, int cols);

void drawMaze(Maze m, int rows, int cols);

void initListCell(ListCell *list);

void freeListCell(ListCell *list);  

void saveToList(ListCell *list, Cell *c);

void removeFromList(ListCell *list, int index);

ArrayOfBool checkNeighbours(Maze m, int rows, int cols, const Cell *c);

void deleteWalls(Cell *c1, Cell *c2, Direction dir);

Direction chooseDirection(ArrayOfBool neighbours);

Cell* moveToCell(Maze m, int rows, int cols, const Cell *c, Direction dir);

Cell* theMRC(ListCell *list);

Cell* randomList(ListCell *list);

Cell* lastNList(ListCell *list, int N);

Cell* lastORrand(ListCell *list, int pLast, int pRand);

bool theMRC_val(const ListCell *list, Cell **out);

#endif