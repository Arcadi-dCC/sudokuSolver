#ifndef file_H
#define file_H

#include "platformTypes.h"

#define FILE_OK 0
#define FILE_DIR_EXISTS 1
#define FILE_PATH 2
#define FILE_STATEMENT 3
#define FILE_COPY 4
#define FILE_NOK 5

#define FILE_DIR_STR_SIZE 50

#define FILE_ARCHIVE_PATH ".\\archive\\"
#define FILE_ARCHIVE_PATH_LEN 10 /* consider '\\' as 1 character */

#define FILE_STATEMENT_NAME "statement.txt"
#define FILE_SOLUTION_NAME "solution.txt"

uint8 fileGetStatement(uint8 row, uint8 col, uint8 dep, uint8 statement_m[row][col][dep]);
uint8 fileArchive(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep]);

#endif // file_H
