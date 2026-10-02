
/*
Main program for the "Labyrinth" project (Growing Tree algorithm).

Features:
    - Prompts the user for maze size, algorithm variant, and starting cell
    - Generates a perfect maze using the Growing Tree family of strategies:
        1) theMRC       - pick the most recent cell (recursive backtracking)
        2) randomList   - pick any active cell randomly
        3) lastNList    - pick a random cell among the last N pushed
        4) lastOrRand   - weighted mix between most-recent and random 

    - Lets the user either:
        - print the maze as ASCII
        - play a small interactive game to find the exit

Notes: 
    - See types.h for definition of Maze, Cell, Direction, constants
    - See fonctions.c for helper functions 

*/


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#include "types.h"
#include "fonctions.h"
#include "game_ncurses.h"

int main(void) {
    Maze m;         // 2D grid of Cell             
    ListCell list;  // dynamic list of active cells used by the Growing Tree algorithm
    srand((unsigned)time(NULL));    

    /* ------------------------------------------------------------
        Step 1: Ask the user for maze dimensions (rows x cols)
                Keep asking until values is valid.
       ------------------------------------------------------------ */
    
    int rows, cols;
    do {
        printf("Please enter the size of row you want (1..%d): ", MAX);
        scanf("%d", &rows);
        if (rows < 1 || rows > MAX) puts("Out of range!! Please type again.");
    } while (rows < 1 || rows > MAX);

    do {
        printf("Please enter the size of column you want (1..%d): ", MAX);
        scanf("%d", &cols);
        if (cols < 1 || cols > MAX) puts("Out of range!! Please type again.");
    } while (cols < 1 || cols > MAX);

    /* ------------------------------------------------------------
        Step 2: Let the user choose the cell-selection strategy.
                This choice shapes the maze style.
       ------------------------------------------------------------ */

    printf("\n=== Maze Generation Algorithms ===\n");
    printf("1. theMRC (Last Cell Only)\n");
    printf("2. randomList (Fully Random Selection)\n");
    printf("3. lastNList (Choose from last N cells)\n");
    printf("4. lastORrand (Weighted combination of theMRC and randomList)\n");
    printf("Choose an algorithm you want (1-->4): ");

    int choice;
    scanf("%d", &choice);

    // Parameters for some variants
    int N = 0;      //used by lastNList       
    int pLast = 3;  // default weight for most-recent in lastOrRand   
    int pRand = 0;  // default weight for random in lastOrRand

    switch (choice) {
        case 1:
            printf("\nYou chose: theMRC\n");
            break;

        case 2:
            printf("\nYou chose: randomList\n");
            break;

        case 3: {
            printf("\nYou chose: lastNList\n");
            
            do {
                printf("Enter N (e.g., 3, 5...): ");
                scanf("%d", &N);
                if (N > (rows * cols))
                    printf("\nInvalid!! Please choose N smaller.\n");
            } while (N > (rows * cols));
            break;
        }

        case 4: {
            printf("\nYou chose: lastORrand\n");
            
            do {
                printf("Enter pRand (priority for random): ");
                scanf("%d", &pRand);
                printf("Enter pLast (priority for theMRC, pLast > pRand + 2): ");
                scanf("%d", &pLast);
                if (pLast <= (pRand + 2))
                    printf("\nInvalid!! Please type again.\n");
            } while (pLast <= (pRand + 2));
            break;
        }

        default:
            printf("\nInvalid choice!! Defaulting to theMRC...\n");
            break;
    }

    /* ------------------------------------------------------------
        Step 3: Ask for the starting cell coordinates (y, x)
                This cell is marked visited and pushed into the list.
       ------------------------------------------------------------ */

    int x, y;
    do {
        printf("Please enter the coordinate of row to start (0..%d): ", rows - 1);
        scanf("%d", &y);
        if (y < 0 || y >= rows) puts("Out of range!! Please type again.");
    } while (y < 0 || y >= rows);

    do {
        printf("Please enter the coordinate of column to start (0..%d): ", cols - 1);
        scanf("%d", &x);
        if (x < 0 || x >= cols) puts("Out of range!! Please type again.");
    } while (x < 0 || x >= cols);

    /* ------------------------------------------------------------
        Step 4: Initialize the maze grid and the active list.
                Seed the algorithm with the starting cell.
       ------------------------------------------------------------ */
    
    initMaze(m, rows, cols);
    initListCell(&list);
   
    Cell *c = &m[y][x];
    c->visited = true; 
    saveToList(&list, c);

    /* ------------------------------------------------------------
        Step 5: Growing Tree loop

        While the active list is not empty:
            1) Select an active cell according to the chosen strategy 
            2) Check for any unvisited neighbors
            3) If at least one:
                - Pick one of the available directions at random
                - Delete the wall between current cell and neighbor
                - Mark neighbor visited and push it into the list
            
               Else:
                - Remove this cell from the active list  
       ------------------------------------------------------------ */

    while (list.nbCell != 0) {
        Cell *c1 = NULL;
        switch (choice) {
            case 1:
                c1 = theMRC(&list);
                break;

            case 2:
                c1 = randomList(&list);
                break;

            case 3: 
                
                c1 = lastNList(&list, N);
                break;

            case 4: 
                c1 = lastORrand(&list, pLast, pRand);
                break;
            
            default:
                c1 = theMRC(&list);
                break;
        }
        if (!c1) break;
        
        ArrayOfBool neighbours = checkNeighbours(m, rows, cols, c1);
        Direction dir = chooseDirection(neighbours); 

        if (dir != NO_DIR) {
            Cell *c2 = moveToCell(m, rows, cols, c1, dir);
            deleteWalls(c1, c2, dir);
            c2->visited = true;
            saveToList(&list, c2);

        } else {
            int index = -1;
            
            for (int i = 0; i < list.nbCell; i++) {
                if (list.cells[i] == c1) {
                    index = i;
                    break;
                } 
            }

            if (index >= 0) {
                removeFromList(&list, index);
            }
        }
    }

    freeListCell(&list);
    puts("Maze generation completed!!");

    /* ------------------------------------------------------------
        Step 6: Post-generation mode
            (1) Print ASCII maze
            (2) Launch ncurses game to find the exit
       ------------------------------------------------------------ */

    int action = 1;
    do {
        printf("\n=== Which mode do you want to do ===\n");
        printf("1. View the maze only\n");
        printf("2. Game of finding the exit of our Maze\n");
        printf("Your selection (1-->2): ");
        scanf("%d", &action);

        if (action != 1 && action != 2) puts("Please just choose 1 or 2!!");
    } while (action != 1 && action != 2);

    switch (action) {
        case 1:
            drawMaze(m, rows, cols);
            break;

        case 2:
            manipulation(m, rows, cols, y, x);
            break;
    }

    return 0;

}    