#ifndef solve_H
#define solve_H

#include "platformTypes.h"

#define SOLVE_OK 0
#define SOLVE_NOK 1

uint8 solveSudoku(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep]);

#endif // solve_H
