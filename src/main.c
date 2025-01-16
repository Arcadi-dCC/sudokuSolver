#define PROGRAM_VERSION "1.0.0"

#include "platformTypes.h"
#include "cmd.h"
#include "file.h"
#include "solve.h"

#define ROWS 9
#define COLS 9
#define DEPTH 11

int main()
{
    cmdTitle(PROGRAM_VERSION);
    
    uint8 sudoku[ROWS][COLS][DEPTH];
    
    cmdDrawGrid();

    if(fileGetStatement(ROWS, COLS, DEPTH, sudoku))
    {
        cmdPrintMessage(0,16,"There is something wrong with the statement file. Please review it.\n");
        cmdExitMessage();
        return 1;
    }
    
    cmdDrawStatement(ROWS, COLS, DEPTH, sudoku);
    
    if(solveSudoku(ROWS, COLS, DEPTH, sudoku))
    {
        cmdPrintMessage(0,16,"The sudoku could not be solved.\n");
        cmdExitMessage();
        return 2;
    }
    
    if(fileArchive(ROWS, COLS, DEPTH, sudoku))
    {
        cmdPrintMessage(0,16,"The sudoku was not archived correctly.\n");
        cmdExitMessage();
        return 3;
    }
    
    cmdPrintMessage(0,16,"The sudoku was stored in the archive.\n");
    cmdExitMessage();
    return 0;
}
