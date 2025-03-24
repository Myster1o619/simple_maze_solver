#include "raylib.h"
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>

#define ROWS 10
#define COLS 10
#define CELL_SIZE 40
#define OFFSET 20

// ***RANDOMIZED START POSITION***
//int startX = rand() % COLS;
//int startY = rand() % ROWS;
//dfs(startX, startY);
// ***RANDOMIZED START POSITION***

typedef struct Tile_t
{
    int x;
    int y;
    bool visited;
    bool walls[4]; //top, right, bottom, left
    //bool moves[4]; //top, right, bottom, left => (true = move made, false = move undone)
    int moves[4]; // 1 = move made; -1 = move undone; 0 = no move
    bool is_solution; // true if this cell is part of the solution path
} Tile_t;

Tile_t grid[ROWS][COLS];

//Directions: top, right, bottom, left
int dirX[4] = { 0, 1, 0, -1 };
int dirY[4] = { -1, 0, 1, 0};
//Directions:
//The indices of these arrays correspond to the following directions :
//
//Index 0 : Top(or Up)
//
//dirX[0] = 0 (no change in the X - coordinate)
//
//dirY[0] = -1 (move up by subtracting 1 from the Y - coordinate)
//
//Index 1: Right
//
//dirX[1] = 1 (move right by adding 1 to the X - coordinate)
//
//dirY[1] = 0 (no change in the Y - coordinate)
//
//Index 2: Bottom(or Down)
//
//dirX[2] = 0 (no change in the X - coordinate)
//
//dirY[2] = 1 (move down by adding 1 to the Y - coordinate)
//
//Index 3: Left
//
//dirX[3] = -1 (move left by subtracting 1 from the X - coordinate)
//
//dirY[3] = 0 (no change in the Y - coordinate)



void draw_grid();
void draw_rectangle_multicolor(int posX, int posY, int width, int height, Color top, Color right, Color bottom, Color left);
void create_exit_and_entrance();
void initialize_grid();
bool is_valid(int x, int y);
void remove_wall(int x1, int y1, int x2, int y2);
bool can_move_to_cell(int x1, int y1, int x2, int y2);
void dfs(int x, int y);
void reset_visited();
bool solve_maze(int x, int y);

void reset_visited()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++) {
            grid[i][j].visited = false;
        }
    }
}

void dfs(int x, int y)
{
    grid[y][x].visited = true;

    int directions[4] = { 0,1,2,3 };

    for (int i = 0; i < 4; i++)
    {
        int r = rand() % 4;
        int temp = directions[i];
        directions[i] = directions[r];
        directions[r] = temp;
    }

    for (int i = 0; i < 4; i++)
    {
        int newX = x + dirX[directions[i]]; // new column index
        int newY = y + dirY[directions[i]]; // new row index

        if (is_valid(newX, newY) && !grid[newY][newX].visited)
        {
            remove_wall(x, y, newX, newY);
            dfs(newX, newY);
        }
    }
}

bool solve_maze(int x, int y)
{
    //For each direction :

    //If there is a cell in that direction, there is no wall blocking you, and that cell hasn't been visited:
    //    Draw a move between the current cell and that cell
    //    Call solve_maze recursively to move to that cell.If that cell returns True, 
    //    then just return True and don't worry about the other directions.
    //    Otherwise, draw an "undo" move between the current cell and the next cell


    // BASE CHECK: check if current cell is the goal
    if (x == ROWS - 1 && y == COLS - 1)
    {
        grid[y][x].is_solution = true;
        return true; //goal reached
    }
    
    grid[y][x].visited = true;

    int directions[4] = { 0,1,2,3 };

    for (int i = 0; i < 4; i++)
    {
        int r = rand() % 4;
        int temp = directions[i];
        directions[i] = directions[r];
        directions[r] = temp;
    }

    for (int i = 0; i < 4; i++)
    {
        int dir = directions[i];
        
        int newX = x + dirX[directions[i]]; // new column index
        int newY = y + dirY[directions[i]]; // new row index

        if (is_valid(newX, newY) && !grid[newY][newX].visited && can_move_to_cell(x, y, newX, newY))
        {
            //mark move as made:
            grid[y][x].moves[dir] = 1;
            printf("Move made: (%d, %d) -> (%d, %d)\n", x, y, newX, newY);
            
            
            //mark move:
            //grid[y][x].moves[directions[i]] = true;
            //printf("Move made: (%d, %d) -> (%d, %d)\n", x, y, newX, newY);

            if (solve_maze(newX, newY))
            {
                grid[y][x].is_solution = true;
                printf("Solution path: (%d, %d) -> (%d, %d)\n", x, y, newX, newY);
                return true; // goal found, propogate the result
            }
            else {
                //undo move;
                //grid[y][x].moves[directions[i]] = false;
                grid[y][x].moves[dir] = -1;
                grid[y][x].is_solution = false;
                printf("Move undone: (%d, %d) -> (%d, %d)\n", x, y, newX, newY);
            }
            
        }
            
    }
    return false;
}

bool can_move_to_cell(int x1, int y1, int x2, int y2)
{
    /*
    int dx = x2 - x1; // Difference in x-coordinates
    int dy = y2 - y1; // Difference in y-coordinates

    if (dx == 1) { // Move right
        return !grid[y1][x1].walls[1]; // Check right wall of current cell
    }
    else if (dx == -1) { // Move left
        return !grid[y1][x1].walls[3]; // Check left wall of current cell
    }
    else if (dy == 1) { // Move down
        return !grid[y1][x1].walls[2]; // Check bottom wall of current cell
    }
    else if (dy == -1) { // Move up
        return !grid[y1][x1].walls[0]; // Check top wall of current cell
    }

    return false; // Invalid move (not a neighbor)
    */

    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dx == 1) //The neighbor is to the right of the current cell.
    {
        // check: right wall current cell and left wall next cell
        if (grid[y1][x1].walls[1] == false && grid[y2][x2].walls[3] == false)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (dx == -1) //The neighbor is to the left of the current cell.
    {
        // check: left wall current cell and right wall next cell
        if (grid[y1][x1].walls[3] == false && grid[y2][x2].walls[1] == false)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (dy == 1) //The neighbor is below the current cell.
    {
        // check bottom wall current cell and top wall next cell
        if (grid[y1][x1].walls[2] == false && grid[y2][x2].walls[0] == false)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (dy == -1) //The neighbor is above the current cell.
    {
        //check top wall current cell and bottom wall next cell
        if (grid[y1][x1].walls[0] == false && grid[y2][x2].walls[2] == false)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

void remove_wall(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    /*int x1, int y1 : Coordinates of the current cell.

    int x2, int y2 : Coordinates of the neighboring cell.*/

    //Based on the values of dx and dy, the function determines which wall to remove :

    //    If dx == 1 : The neighbor is to the right of the current cell.

    //    If dx == -1 : The neighbor is to the left of the current cell.

    //    If dy == 1 : The neighbor is below the current cell.

    //    If dy == -1 : The neighbor is above the current cell.

    //bool walls[4]; 
    //top, right, bottom, left
    /*
        0 = top
        1 = right
        2 = bottom
        3 = right
    */

    if (dx == 1) //The neighbor is to the right of the current cell.
    {
        grid[y1][x1].walls[1] = false; // right wall current cell
        grid[y2][x2].walls[3] = false; // left wall of next cell

    }
    else if (dx == -1) //The neighbor is to the left of the current cell.
    {
        grid[y1][x1].walls[3] = false; // left wall of current cell
        grid[y2][x2].walls[1] = false; // right wall of next cell
    }
    else if (dy == 1) //The neighbor is below the current cell.
    {
        grid[y1][x1].walls[2] = false; // bottom wall of current cell
        grid[y2][x2].walls[0] = false; // top wall of next cell
    }
    else if (dy == -1) //The neighbor is above the current cell.
    {
        grid[y1][x1].walls[0] = false; // top wall of current cell
        grid[y2][x2].walls[2] = false; // bottom wall of next cell
    }
}

bool is_valid(int x, int y)
{
    return x >= 0 && x < COLS && y >= 0 && y < ROWS;
}

void initialize_grid()
{
    // Initialize the grid positions
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++) {
            grid[i][j].x = j * CELL_SIZE;
            grid[i][j].y = i * CELL_SIZE;
            grid[i][j].visited = false;
            grid[i][j].is_solution = false;
            for (int k = 0; k < 4; k++)
            {
                grid[i][j].walls[k] = true;
                grid[i][j].moves[k] = 0;
            }
        }
    }
}

void create_exit_and_entrance()
{
    //top left cell is the start (break top wall)
    grid[0][0].walls[0] = false;
     
    //bottom right cell is the end (break bottom wall)
    grid[ROWS - 1][COLS - 1].walls[2] = false;
}

void draw_rectangle_multicolor(int posX, int posY, int width, int height, Color top, Color right, Color bottom, Color left)
{
    //DrawRectangle(posX, posY, width, height, BLACK); // Fill the cell with black
    
    if (top.a != 0) DrawLine(posX, posY, posX + width, posY, top);
    if (right.a != 0) DrawLine(posX + width, posY, posX + width, posY + height, right);
    if (bottom.a != 0) DrawLine(posX, posY + height, posX + width, posY + height, bottom);
    if (left.a != 0) DrawLine(posX, posY, posX, posY + height, left);

    // Draw top side
    //DrawLine(posX, posY, posX + width, posY, top);

    // Draw right side
    //DrawLine(posX + width, posY, posX + width, posY + height, right);

    // Draw bottom side
    //DrawLine(posX, posY + height, posX + width, posY + height, bottom);

    // Draw left side
    //DrawLine(posX, posY, posX, posY + height, left);
}

void draw_grid()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            int posX = grid[i][j].x;
            int posY = grid[i][j].y;

            // Draw walls
            Color top = grid[i][j].walls[0] ? BLACK : WHITE;
            Color right = grid[i][j].walls[1] ? BLACK : WHITE;
            Color bottom = grid[i][j].walls[2] ? BLACK : WHITE;
            Color left = grid[i][j].walls[3] ? BLACK : WHITE;

            draw_rectangle_multicolor(posX + OFFSET, posY, CELL_SIZE, CELL_SIZE, top, right, bottom, left);

            // Calculate the center of the current cell
            int centerX = posX + OFFSET + CELL_SIZE / 2;
            int centerY = posY + CELL_SIZE / 2;

            //draw moves
            for (int dir = 0; dir < 4; dir++)
            {
                int nextCenterX = centerX + dirX[dir] * CELL_SIZE;
                int nextCenterY = centerY + dirY[dir] * CELL_SIZE;

                if (grid[i][j].moves[dir] == 1)
                {
                    //solution path
                    DrawLine(centerX, centerY, nextCenterX, nextCenterY, RED);
                }
                else if (grid[i][j].moves[dir] == -1)
                {
                    //backtracked path
                    DrawLine(centerX, centerY, nextCenterX, nextCenterY, GRAY);
                }
            }
        }
    }
}

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 800;

    srand(time(NULL)); //seed random number generator

    initialize_grid();
    dfs(0, 0); // Generate the maze using DFS
    create_exit_and_entrance();
    reset_visited();
    bool can_solve_maze = solve_maze(0, 0);


    InitWindow(screenWidth, screenHeight, "raylib example");

    SetTargetFPS(60); // Set the game to run at 60 frames per second



    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        draw_grid();
        // Solve the maze (if not already solved)
        //if (!can_solve_maze) {
        //    can_solve_maze = solve_maze(0, 0); // Start solving from (0, 0)
        //}


        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}