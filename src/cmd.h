#ifndef cmd_H
#define cmd_H

//Offset for the sudoku drawings
#define OFFSET_X 0
#define OFFSET_Y 3

#include "platformTypes.h"

void cmdSetCursor(uint32 x, uint32 y);
void cmdDrawGrid(void);
void cmdDrawStatement(uint8 row, uint8 col, uint8 dep, uint8 statement[row][col][dep]);
void cmdDrawOneNumber(uint8 x, uint8 y, uint8 number);
void cmdPrintMessage(uint32 x, uint32 y, const char* msg);
void cmdExitMessage(void);
void cmdTitle(char* prog_ver);

#endif // cmd_H
