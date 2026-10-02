
/*
ncurses-bases interactive game for the "Labyrinthe" project.

Responsibilities:
    - Render the maze as ASCII using ncurses (window coordinates differ from grid coordinates)
    - Let the player move with arrow keys while respecting maze wall
    - Optional "mark mode"  to drop/remove X
    - Detect victory when player reaches the goal cell

Coordinate systems:
    - Maze coordinates: (row, col) in  [0..row-1] x [0..cols-1]
    - Screen coordinates: (sy, sx) converted to that each cell is a 3x1 interior framed by walls.

Key binding:
    - Arrow keys: move (if wall allows)
    - x: enable mark mode (leave X on visited cells)
    - n: disable mark mode 
    - q: quit the game
*/


#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include "types.h"
#include "fonctions.h"
#include "game_ncurses.h"

/*
Convert a maze(row,col) into screen (sy,sx) for the cell interior.
Layout:
    Row i interior lines appear at sy = 1 + 2*i
    Col j interior columns appear at sx = 2 + 4*j
This matches a cell drawn as:
    +---+ (walls on even rows)
    |   | (interior on odd rows)
*/

static void intoScreen(int row, int col, int *sy, int *sx) {
    *sy = 1 + 2*row;    
    *sx = 2 + 4*col;    
}

/*
Check if the player can pass from (y,x) in the given direction.
Conditions:
    - Direction is valid
    - No wall on the current cell side
    - Target cell stays inside the maze bounds
*/


static bool canPass(Maze m, int rows, int cols, int y, int x, Direction dir) {
    if (dir == NO_DIR) return false;

    if (m[y][x].walls[dir] == true) return false;

    int ny = y, nx = x;
    if (dir == NORTH) ny--;
    if (dir == SOUTH) ny++;
    if (dir == WEST)  nx--;
    if (dir == EAST)  nx++;

    if (ny < 0 || ny >= rows || nx < 0 || nx >= cols) return false;

    return true;
}

/*
Draw the entire maze using ncurses.
We render: 
    - Top wall row
    - For each grid row:
        - Left border + cell interiors + vertical walls
        - Bottom walls
Interior is 3 spaces wide; walls use '+' '-' and '|'.
*/

static void drawMaze_ncurses(Maze m, int rows, int cols) {
    clear();

    int y = 0, x = 0;

    // Draw the topmost horizontal walls
    for (int col = 0; col < cols; col++) {  
        mvaddch(y, x, '+');
        x++;

        if (m[0][col].walls[NORTH] == true) {
            mvaddch(y, x, '-');
            mvaddch(y, x+1, '-');
            mvaddch(y, x+2, '-');
        } else {
            mvaddch(y, x, ' '); 
            mvaddch(y, x+1, ' '); 
            mvaddch(y, x+2, ' ');
        }
        x += 3;
    }
    mvaddch(y, x, '+');
    

    // For each row: interiors + east walls, then south walls
    for (int row = 0; row < rows; row++) {   
        y = 1 + 2*row; 
        x = 0;

        // West border of the first cell
        if (m[row][0].walls[WEST] == true) {
            mvaddch(y, x, '|');
        } else {
            mvaddch(y, x, ' ');
        }
        x++;

        // Interior (3 spaces) then east wall for each cell
        for (int col = 0; col < cols; col++) {
            mvaddch(y, x, ' ');
            mvaddch(y, x+1, ' ');
            mvaddch(y, x+2, ' ');
            x += 3;

            if (m[row][col].walls[EAST] == true) {
                mvaddch(y, x, '|');
            } else {
                mvaddch(y, x, ' ');
            }
            x++;
        }

        // Bottom walls for this row
        y = 2 + 2*row; x = 0; //y := y + 1 de ve south cua o
        for (int col = 0; col < cols; col++) {
            mvaddch(y, x, '+');
            x++;
            if (m[row][col].walls[SOUTH] == true) {
                mvaddch(y, x, '-'); 
                mvaddch(y, x+1, '-'); 
                mvaddch(y, x+2, '-');
            } else {
                mvaddch(y, x, ' '); 
                mvaddch(y, x+1, ' '); 
                mvaddch(y, x+2, ' ');
            }
            x += 3;
        }
        mvaddch(y, x, '+');
    }
}

/*
Render a single cell's interior character based on priority:
    - 'D' for player position (py,px)
    - 'G' for goal position (gy,gx)
    - 'X' if marked 
    - ' ' otherwise
*/

static void render_cell(int row, int col, int py, int px, int gy, int gx, const bool *marks, int cols) {
    int sy, sx; 
    intoScreen(row, col, &sy, &sx);
    
    char ch = ' ';
    if (row == py && col == px) ch = 'D';
    else if (row == gy && col == gx) ch = 'G';
    else if (marks[row*cols + col]) ch = 'X'; 

    mvaddch(sy, sx, ch);
}

/* 
Turn ON markmode: subsequent moves will drop 'X'
*/

static void mark_Mode_On(bool *marking, int rows) {
    *marking = true;

    mvprintw(2*rows + 7, 0, "Mark mode: ON. Press n to turn this mode off!!"); 
    refresh(); 
}

/*
Turn OFF mark mode:
    - Clear any 'X' under player 
    - Keep existing 'X' elsewhere 
*/

static void mark_Mode_Off(bool *marking, int pY, int pX, bool *marks, int *top, int *stackY, int *stackX, int cols, int rows, int gy, int gx) {
    *marking = false;

    if (marks[pY*cols + pX]) {
        marks[pY*cols + pX] = false;
        render_cell(pY, pX, pY, pX, gy, gx, marks, cols);   
    }

    mvprintw(2*rows + 7, 0, "Mark mode: OFF. Press x to turn this mode on!! "); 
    refresh(); 
}

/*
Public entry point call from growingTree.c
Initializes ncurses, draws the maze , handles input/game loop, and cleans up.
*/

void manipulation(Maze m, int rows, int cols, int startY, int startX) {
    // --- ncurses setup ---
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    timeout(20);
    curs_set(0);    //hide cursor

    drawMaze_ncurses(m, rows, cols);

    // Player position and goal (goal = bottom-right corner)
    int pY = startY;
    int pX = startX;

    const int gy = rows - 1, gx = cols - 1;

    // 'X' marks and simple stack to support backtracking visualization
    bool *marks = (bool*)calloc((size_t)rows * (size_t)cols, sizeof(bool));
    int *stackY = (int*)malloc((size_t)rows * (size_t)cols * sizeof(int));
    int *stackX = (int*)malloc((size_t)rows * (size_t)cols * sizeof(int));
    int top = -1; 
    bool marking = false;   // mark mode is OFF initially 
    bool won = false; 

    if (!marks || !stackY || !stackX) {
        // Allocation failure : exit
        endwin();

        free(marks);
        free(stackY);
        free(stackX);

        return;
    } 

    // Initial paint of all cells
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            render_cell(i, j, pY, pX, gy, gx, marks, cols);

    // HHD / instructions
    mvprintw(2*rows + 5, 0, "Please use arrow keys to move and press 'q' to quit!!");
    mvprintw(2*rows + 7, 0, "Mark mode: OFF. Press x to turn this mode on!!");
    refresh();

    // int ch; 
    int ch;
    while ((ch = getch()) != 'q') {

        if (won) {         
            // After victory, ignore all keys except 'q'      
            continue;       
        }      

        // Toggle mark mode
        if (ch == 'x' || ch == 'X') {
            mark_Mode_On(&marking, rows);
            continue;
        }
        if (ch == 'n' || ch == 'N') {
            mark_Mode_Off(&marking, pY, pX, marks, &top, stackY, stackX, cols, rows, gy, gx);
            continue;
        }

        // Map key to direction
        Direction dir = NO_DIR;
        if (ch == KEY_UP)    dir = NORTH;
        if (ch == KEY_DOWN)  dir = SOUTH;
        if (ch == KEY_LEFT)  dir = WEST;
        if (ch == KEY_RIGHT) dir = EAST;

        // Ignore non-direction keys
        if (dir == NO_DIR) continue;
        if (canPass(m, rows, cols, pY, pX, dir) == false) continue;

        // Check wall
        int ny = pY, nx = pX;
        if (dir == NORTH) ny--;
        if (dir == SOUTH) ny++;
        if (dir == WEST)  nx--;
        if (dir == EAST)  nx++;


        // Mark mode logic:
        //  - If moving back to the top of stack cell, pop and clear X under current cell 
        //  - Else, push current cell and drop an X on it   
        if (marking == true) {
            if (top >= 0 && stackY[top] == ny && stackX[top] == nx) {
                // backtrack
                if (marks[pY*cols + pX]) {
                    marks[pY*cols + pX] = false;
                    render_cell(pY, pX, pY, pX, gy, gx, marks, cols); 
                }
                top--;
            } else {
                // Forward
                if (!marks[pY*cols + pX]) {
                    marks[pY*cols + pX] = true;
                    render_cell(pY, pX, pY, pX, gy, gx, marks, cols); 
                }

                stackY[++top] = pY;
                stackX[top]   = pX;
            }
        }

        // Erase old player 'D' by re-rendering that interior without player coords
        render_cell(pY, pX, -1, -1, gy, gx, marks, cols); 

        // Move player
        pY = ny;
        pX = nx;

        // Draw player at new position
        render_cell(pY, pX, pY, pX, gy, gx, marks, cols);
        refresh();

        // Victory check
        if (pY == gy && pX == gx) {
            won = true;
            mvprintw(2*rows + 9, 0, "Victory!! Press q to exit...");
            refresh();
        }
    }

    // --- Cleanup ---
    endwin();

    free(marks);
    free(stackY);
    free(stackX);
}
