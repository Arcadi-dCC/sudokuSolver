#include "cmd.h"

#include <stdio.h>
#include <windows.h>

//Sets the cmd cursor on a specific position given x (horizontal) and y (vertical) coordinates.
void cmdSetCursor(uint32 x, uint32 y)
{
    HANDLE handle;
    COORD coordinates;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    coordinates.X = x;
    coordinates.Y = y;
    SetConsoleCursorPosition(handle, coordinates);
}

//Shows the name of the program and author, the version, and the year of release at the top of the screen.
void cmdTitle(char* prog_ver)
{
    printf("Sudoku Solver [Version ");
    printf("%s", prog_ver);
    printf("]\n2025 ADCC.");
}

//Draws a simple sudoku grid
void cmdDrawGrid(void)
{
    sint16 x=0;
    sint16 y=0;

    //Draw horizontal divisions
    for(y=3;y<8;y=y+4)
    {
        for(x=0;x<11;x++)
        {
            if((x+1)%4 == 0)
            {
                continue;
            }
            cmdSetCursor(x+OFFSET_X,y+OFFSET_Y);
            printf("-");
        }
    }

    //Draw vertical divisions
    for(x=3;x<8;x=x+4)
    {
        for(y=0;y<11;y++)
        {
            if((y+1)%4 == 0)
            {
                continue;
            }
            cmdSetCursor(x+OFFSET_X,y+OFFSET_Y);
            printf("|");
        }
    }
}

//Draws the initial known numbers in the cmd grid. Avoids zeroes (i.e. unknown values)
void cmdDrawStatement(uint8 row, uint8 col, uint8 dep, uint8 statement[row][col][dep])
{
    uint8 x=0;
    uint8 y=0;

    for(y=0;y<9;y++)
    {
        for(x=0;x<9;x++)
        {
            if(statement[x][y][0])
            {
                cmdSetCursor((uint32)(x+x/3+OFFSET_X), (uint32)(y+y/3+OFFSET_Y));
                printf("%d", statement[x][y][0]);
            }
        }
    }
}

void cmdDrawOneNumber(uint8 x, uint8 y, uint8 number)
{
    cmdSetCursor((uint32)(x+x/3+OFFSET_X), (uint32)(y+y/3+OFFSET_Y));
    printf("%d", number);
}

void cmdPrintMessage(uint32 x, uint32 y, const char* msg)
{
     cmdSetCursor(x, y);
     printf("%s", msg);
}

void cmdExitMessage(void)
{
     printf("Press enter to exit...");
     (void)getchar();
}
