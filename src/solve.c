#include "solve.h"
#include "cmd.h"

#define TOTAL_CELLS 81

//Declaration of functions that are used locally only.
void solveQuadrantOffset(uint8 qdr, uint8* offset_x, uint8* offseet_y);
void solveCleanFuzzySudoku(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep]);
uint8 solveByCellDiscard(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr, uint8* missing);
uint8 solveByNumberDiscard(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr, uint8* missing);
uint8 solveFindMissingNumbers(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr, uint8* missing);
uint8 solveByFuzzySolutions(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep]);
uint8 solveByFuzzyNumberDiscard(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr);
uint8 solveByFuzzyCellDiscard(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr);

//Solves the sudoku passed as a reference and displays the result in cmd in real time.
uint8 solveSudoku(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep])
{
    uint8 filled_cells=0;
    uint8 past_filled_cells = 0;
    uint8 iteration=0;
    uint8 statement_cells=0; //Number of cells already filled by the statement.
    uint8 x=0;
    uint8 y=0;

    //Count how many cells are already filled by the statement.
    for(y=0;y<9;y++)
    {
        for(x=0;x<9;x++)
        {
            if(sudoku[x][y][0]!=0)
            {
                statement_cells++;
            }
        }
    }
    //update filled_cells
    filled_cells=statement_cells;

    uint8 qdr=0U;
    uint8 missing[] = {0,0,0,0,0,0,0,0,0,0}; //For numbers 1 to 9. Position 0 is not used


    while(filled_cells<TOTAL_CELLS)
    {
        //Maximum number of iterations to avoid an infinite loop on unsolvable sudokus.
        //Justification of the value: in order to avoid an infinite loop, at least one new number should be found in each iteration.
        //If as much iterations as missing numbers are completed and the sudoku is still unsolved, an infinite loop has appeared.
        if(iteration + statement_cells == TOTAL_CELLS)
        {
            return SOLVE_NOK;
        }

        past_filled_cells = filled_cells;

        //Try to solve for each quadrant

        for(qdr=0U;qdr<9U;qdr++)
        {
            //1st - Identify missing numbers in the quadrant
            if(solveFindMissingNumbers(row, col, dep, sudoku, qdr, missing))
            {
                //If there are any missing numbers:

                //2nd - Try to solve in the quadrant by cell discard
                filled_cells = filled_cells + solveByCellDiscard(row, col, dep, sudoku, qdr, missing);

                //3rd - Try to solve in the quadrant by number discard
                filled_cells = filled_cells + solveByNumberDiscard(row, col, dep, sudoku, qdr, missing);
            }
        }

        //4th - If the discard methods haven't found any new solution, try to solve considering fuzzy solutions
        if(past_filled_cells == filled_cells)
        {
            filled_cells = filled_cells + solveByFuzzySolutions(row, col, dep, sudoku);
        }

        iteration++;
    }

    return SOLVE_OK;
}

/*Quadrant disposition*/
/*      0|1|2         */
/*      - - -         */
/*      3|4|5         */
/*      - - -         */
/*      6|7|8         */

//Returns the x and y offset for the specified quadrant
void solveQuadrantOffset(uint8 qdr, uint8* offset_x, uint8* offset_y)
{
    *offset_x = (qdr - (qdr/3U)*3U)*3U; //Dividing and then multiplying the quadrant by 3 is not superfluous. The loss of data from the integer division is useful.
    *offset_y = qdr - qdr%3U;
}

//Searches numbers that still are not present in a quadrant.
//Returns quantity of missing numbers
//Also returns the list of missing numbers through missing[10] array. Missing [0] is not used and always reads 0.
uint8 solveFindMissingNumbers(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr, uint8* missing)
{
    //Determine offset for current quadrant
    uint8 x=0, offset_x=0, y=0, offset_y=0, i=0;
    solveQuadrantOffset(qdr, &offset_x, &offset_y);

    //Prepare a register array. 1 means the number is missing. Initially, all are missing by default (except 0)
    for(i=1;i<10;i++)
    {
        missing[i]=1;
    }

    uint8 missing_ctr=9; //Missing numbers counter. Initially, all are assumed to be.

    for(i=1;i<10;i++) //For each number
    {
        for(y=0;y<3;y++)
        {
            for(x=0;x<3;x++) //In each cell
            {
                if(sudoku[x+offset_x][y+offset_y][0] == i)
                {
                    missing[i] = 0U; //Discard the existing numbers.
                    missing_ctr--; //Reduce the missing numbers counter.
                }
            }
        }
    }

    return missing_ctr;
}

//For each missing number, tries to slot it in every free cell of the quadrant.
//If a number can slot in only one cell, they are paired in the solution layer of the sudoku matrix.
//Returns quantity of numbers sloted
uint8 solveByCellDiscard(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr, uint8* missing)
{
    //1 - Obtain quadrant offset
    uint8 offset_x=0, offset_y=0;

    solveQuadrantOffset(qdr, &offset_x, &offset_y);

    uint8 x=0, y=0, t=0, conflict=0, suitable_cells=0, suitable_x=0, suitable_y=0, sloted_qty=0;

    //2 - Retrieve a missing number
    for(uint8 i=1;i<10;i++)
    {
        if(!missing[i])
        {
            continue;
        }

        suitable_cells=0;

    //3 - Look for empty cells (they contain 0)
        for(y=0;y<3;y++)
        {
            for(x=0;x<3;x++)
            {
                if(sudoku[x+offset_x][y+offset_y][0])
                {
                    continue;
                }

    //4 - Check that the number does not exist in the row
                conflict=0;
                for(t=0;t<9;t++)
                {
                    if(sudoku[t][y+offset_y][0] == i)
                    {
                        conflict = 1; //The number already exists
                        break;
                    }
                }

    //5 - If there is no conflict, check that the number does not exist in the column
                if(!conflict)
                {
                    for(t=0;t<9;t++)
                    {
                        if(sudoku[x+offset_x][t][0] == i)
                        {
                            conflict = 1; //The number already exists
                            break;
                        }
                    }
                }

    //6 - If the number could slot in the cell, register this possibility and save its coordinates
                if(!conflict)
                {
                    suitable_cells++;
                    suitable_x=x+offset_x;
                    suitable_y=y+offset_y;
                }
            }
        }
    //7 - If the number can only slot in one cell:
        if(suitable_cells == 1)
        {
            //a. Write the number in the cell.
            sudoku[suitable_x][suitable_y][0] = i;

            //b. Update the missing array
            missing[i]=0;

            //c. Register that a new number has been sloted
            sloted_qty++;

            //d. Write the number on screen.
            cmdDrawOneNumber(suitable_x, suitable_y, i);
        }
    }
    return sloted_qty;
}


//For each free cell, tries to slot every missing number of the quadrant.
//If a cell can only slot one number, they are paired in the solution layer of the sudoku matrix.
//Returns quantity of numbers sloted
uint8 solveByNumberDiscard(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr, uint8* missing)
{
    //1 - Obtain quadrant offset
    uint8 offset_x=0, offset_y=0;

    solveQuadrantOffset(qdr, &offset_x, &offset_y);

    uint8 x=0, y=0, i=0, t=0, conflict=0, sloted_qty=0;

    //2 - Look for an empty slot (they contain 0)
    for(y=0;y<3;y++)
    {
        for(x=0;x<3;x++)
        {
            if(sudoku[x+offset_x][y+offset_y][0])
            {
                continue;
            }

    //3 - Reinitialize the fuzzy solution array for the cell to 0.

            for(i=1;i<11;i++)
            {
                sudoku[x+offset_x][y+offset_y][i] = 0;
            }

    //4 - Retrieve a missing number.
            for(i=1;i<10;i++)
            {
                if(!missing[i])
                {
                    continue;
                }

    //5 - Check that the number does not exist in the row.
                conflict=0;
                for(t=0;t<9;t++)
                {
                    if(sudoku[t][y+offset_y][0] == i)
                    {
                        conflict = 1; //The number already exists.
                        break;
                    }
                }

    //6 - If there is no conflict, check that the number does not exist in the column.
                if(!conflict)
                {
                    for(t=0;t<9;t++)
                    {
                        if(sudoku[x+offset_x][t][0] == i)
                        {
                            conflict = 1; //The number already exists.
                            break;
                        }
                    }
                }

    //7 - If the cell could accommodate this number, register this possibility and save it
                if(!conflict)
                {
                    sudoku[x+offset_x][y+offset_y][i] = 1;
                    sudoku[x+offset_x][y+offset_y][10]++;
                }

            }
    //8 - If the cell can only accommodate one number:
            if(sudoku[x+offset_x][y+offset_y][10] == 1)
            {
                //a. Write the number in the solutions level.
                for(i=1;i<10;i++)
                {
                    if(sudoku[x+offset_x][y+offset_y][i] == 1)
                    {
                        sudoku[x+offset_x][y+offset_y][0] = i;
                        break;
                    }
                }

                //b. Update the missing array
                missing[i]=0;

                //c. Register that a new number has been sloted
                sloted_qty++;

                //d. Write the number on screen.
                cmdDrawOneNumber(x+offset_x, y+offset_y, i);
            }
        }
    }

    return sloted_qty;
}

//Initializes all the layers of the sudoku matrix to zero, except the solution layer.
void solveCleanFuzzySudoku(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep])
{
    uint8 x=0, y=0, i=1;

    for(y=0; y<9; y++)
    {
        for(x=0; x<9; x++)
        {
            for(i=1; i<11; i++)
            {
                sudoku[x][y][i] = 0;
            }
        }
    }
}

//Tries to find numbers from a quadrant that must go in one specific column or row.
//Then solves by the number discard method in the other quadrants, as is those numbers were occupying that column or row.
//Since this method is way more time consuming than the other two, it should only be called when the other methods are not capable of finding a new solution.
uint8 solveByFuzzySolutions(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep])
{
    //1 - Clean the fuzzy solution layers of the sudoku matrix
    solveCleanFuzzySudoku(row, col, dep, sudoku);

    //2 - For each empty cell in each quadrant, save all possible solutions in the fuzzy solutions layers
    uint8 qdr=0, x=0, y=0, offset_x=0, offset_y=0, i=0, t=0, conflict=0, sloted_qty=0;
    uint8 missing[10];
    for(qdr=0;qdr<9;qdr++)
    {
        solveQuadrantOffset(qdr, &offset_x, &offset_y);

        solveFindMissingNumbers(row, col, dep, sudoku, qdr, missing);

        //a. Retrieve a missing number
        for(y=0;y<3;y++)
        {
            for(x=0;x<3;x++)
            {
                if(sudoku[x+offset_x][y+offset_y][0])
                {
                    continue;
                }

                for(i=1;i<10;i++)
                {
                    if(!missing[i])
                    {
                        continue;
                    }

                    conflict = 0;

        //b. Check that the number does not exist in the row.
                    for(t=0;t<9;t++)
                    {
                        if(sudoku[t][y+offset_y][0] == i)
                        {
                            conflict = 1; //The number already exists.
                            break;
                        }
                    }
        //c. If there is no conflict, check that the number does not exist in the column.
                    if(!conflict)
                    {
                        for(t=0;t<9;t++)
                        {
                            if(sudoku[x+offset_x][t][0] == i)
                            {
                                conflict = 1; //The number already exists.
                                break;
                            }
                        }
                    }

        //d. If the cell could accommodate this number, store the possibility in the fuzzy solutions layers.
                    if(!conflict)
                    {
                        sudoku[x+offset_x][y+offset_y][i] = 1;
                        sudoku[x+offset_x][y+offset_y][10]++;
                    }
                }
            }
        }
    }

    //Try to solve considering fuzzy solutions.
    //Allow only for one solution to be found this way (read solveByFuzzyNumberDiscard() description).
    for(qdr=0;qdr<9;qdr++)
    {
        //3 - Try to solve by number discard, considering fuzzy soluions from other quadrants.
        sloted_qty = sloted_qty + solveByFuzzyNumberDiscard(row, col, dep, sudoku, qdr);
        if(sloted_qty)
        {
            break;
        }

        //4 - If the previous did not work, try to solve by cell discard, considering fuzzy solutions from other quadrants.
        sloted_qty = sloted_qty + solveByFuzzyCellDiscard(row, col, dep, sudoku, qdr);
        if(sloted_qty)
        {
            break;
        }

    }

    return sloted_qty;
}

//Finds one solution to the sudoku. This function depends on the work done by solveByFuzzySolutions, which fills the fuzzy solutions layers of the sudoku.
//If the number being tested is found in the fuzzy solutions layers of another quadrant, it looks for it in the other two rows/columns of such quadrant,
//depending on the check being performed. If it is not found, then the tested number gets discarded on the original quadrant.
//The fuzzy solutions layers must be fully updated, and therefore only one solution can be found before having to rewrite all these layers.
uint8 solveByFuzzyNumberDiscard(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr)
{
    //1 - Obtain quadrant offset
    uint8 offset_x=0, offset_y=0;

    solveQuadrantOffset(qdr, &offset_x, &offset_y);

    uint8 x=0, y=0, i=0, t=0, w=0, fuzzy_solution_found=0, sloted_qty=0;

    //2 - Look for an empty slot (they contain 0)
    for(y=0;y<3;y++)
    {
        for(x=0;x<3;x++)
        {
            if(sudoku[x+offset_x][y+offset_y][0])
            {
                continue;
            }

    //3 - Retrieve possible missing numbers for that spot from the fuzzy solutions layers
            for(i=1;i<10;i++)
            {
                if(!sudoku[x+offset_x][y+offset_y][i])
                {
                    continue;
                }

                fuzzy_solution_found = 0; //Assumed to be not found by default

    //4 - Check if a fuzzy solution can be found row-wise

                for(t=0;t<6;t++)
                {
                    //If the same number is possible in the same row of another quadrant
                    if(sudoku[((x/3)*3+3+offset_x+t)%9][y+offset_y][i])
                    {
                        //Check if the number is possible in the other rows of that quadrant. If so, exit the loop before it ends
                        for(w=0;w<6;w++)
                        {
                            if(sudoku[((((x/3)*3+3+offset_x+t)%9)/3)*3+w%3][(y+1+(w/3))%3+offset_y][i])
                            {
                                break;
                            }
                        }

                        //If the loop has reached its final boundary without interruptions, the number is not possible in the other rows.
                        if(w==6)
                        {
                            fuzzy_solution_found = 1;
                            break; //No need to search more: a fuzzy solution has already been found
                        }
                    }
                }

    //5 - If not found yet, check if a fuzzy solution can be found column-wise
                if(!fuzzy_solution_found)
                {
                    for(t=0;t<6;t++)
                    {
                        //If the same number is possible in the same column of another quadrant
                        if(sudoku[x+offset_x][((y/3)*3+3+offset_y+t)%9][i])
                        {
                            //Check if the number is possible in the other columns of that quadrant. If so, exit the loop before it ends
                            for(w=0;w<6;w++)
                            {
                                if(sudoku[(x+1+(w/3))%3+offset_x][((((y/3)*3+3+offset_y+t)%9)/3)*3+w%3][i])
                                {
                                    break;
                                }
                            }

                            //If the loop has reached its final boundary without interruptions, the number is not possible in the other columns.
                            if(w==6)
                            {
                                fuzzy_solution_found = 1;
                                break; //No need to search more: a fuzzy solution has already been found
                            }
                        }
                    }
                }

    //6 - If a fuzzy solution has been found in another quadrant, remove the tested number from the possible solutions in the fuzzy solutions layers
                if(fuzzy_solution_found)
                {
                    sudoku[x+offset_x][y+offset_y][i] = 0;
                    sudoku[x+offset_x][y+offset_y][10]--;
                }

            }

    //7 - If the cell can only accommodate one number:
            if(sudoku[x+offset_x][y+offset_y][10] == 1)
            {
                //a. Write the number in the solutions level.
                for(i=1;i<10;i++)
                {
                    if(sudoku[x+offset_x][y+offset_y][i] == 1)
                    {
                        sudoku[x+offset_x][y+offset_y][0] = i;
                        break;
                    }
                }

                //b. Remove the number from the fuzzy solutions layers
                sudoku[x+offset_x][y+offset_y][i] = 0;
                sudoku[x+offset_x][y+offset_y][10] = 0;

                //c. Write the number on screen.
                cmdDrawOneNumber(x+offset_x, y+offset_y, i);

                //d. Register that a new number has been sloted and return. Only one solution can be found at a time with this method
                sloted_qty++;
                return sloted_qty;
            }
        }
    }

    return sloted_qty;
}

//Finds one solution to the sudoku. This function depends on the work done by solveByFuzzySolutions, which fills the fuzzy solutions layers of the sudoku.

//For each missing number in the quadrant, it tries to slot it in every free cell of that quadrant. If no fuzzy solution for that number are
//found in either the same row or column of another quadrant, the cell being tested is marked as suitable.
//If at the end of both checks there is only one suitable cell for the checked number, cell and number are paired in the solutions layer.

//The fuzzy solutions layers must be fully updated, and therefore only one solution can be found before having to rewrite all these layers.
uint8 solveByFuzzyCellDiscard(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep], uint8 qdr)
{
    uint8 offset_x=0, offset_y=0;
    uint8 missing[10];
    uint8 x=0, y=0, t=0, w=0, suitable_cells=0, suitable_x=0, suitable_y=0, sloted_qty=0;
    uint8 fuzzy_solution_found=0;

    //1 - Obtain quadrant offset
    solveQuadrantOffset(qdr, &offset_x, &offset_y);

    //2 - Obtain the missing numbers from the quadrant
    solveFindMissingNumbers(row, col, dep, sudoku, qdr, missing);

    //3 - Retrieve a missing number
    for(uint8 i=1;i<10;i++)
    {
        if(!missing[i])
        {
            continue;
        }

        suitable_cells=0;

        //3 - Look for cells in the fuzzy solutions layer of the number, in which it could fit according to solveByFuzzySolutions() (they contain 1)
        for(y=0;y<3;y++)
        {
            for(x=0;x<3;x++)
            {
                if(!sudoku[x+offset_x][y+offset_y][i])
                {
                    continue;
                }

                fuzzy_solution_found = 0; //Assumed to be not found by default

                //4 - Check if a fuzzy solution can be found row-wise
                for(t=0; t<6; t++)
                {
                    //If the same number is possible in the same row of another quadrant
                    if(sudoku[((x/3)*3+3+offset_x+t)%9][y+offset_y][i])
                    {
                        //Check if the number is possible in the other rows of that quadrant. If so, exit the loop before it ends
                        for(w=0;w<6;w++)
                        {
                            if(sudoku[((((x/3)*3+3+offset_x+t)%9)/3)*3+w%3][(y+1+(w/3))%3+offset_y][i])
                            {
                                break;
                            }

                            //If the loop has reached its final boundary without interruptions, the number is not possible in the other rows.
                            if(w==6)
                            {
                                fuzzy_solution_found = 1;
                                break; //No need to search more: a fuzzy solution has already been found
                            }
                        }
                    }
                }

                //5 - If not found yet, check if a fuzzy solution can be found column-wise
                if(!fuzzy_solution_found)
                {
                    for(t=0; t<6; t++)
                    {
                        //If the same number is possible in the same column of another quadrant
                        if(sudoku[x+offset_x][((y/3)*3+3+offset_y+t)%9][i])
                        {
                            //Check if the number is possible in the other columns of that quadrant. If so, exit the loop before it ends
                            for(w=0; w<6; w++)
                            {
                                if(sudoku[(x+1+(w/3))%3+offset_x][((((y/3)*3+3+offset_y+t)%9)/3)*3+w%3][i])
                                {
                                    break;
                                }

                                //If the loop has reached its final boundary without interruptions, the number is not possible in the other columns.
                                if(w==6)
                                {
                                    fuzzy_solution_found = 1;
                                    break; //No need to search more: a fuzzy solution has already been found
                                }
                            }
                        }
                    }
                }

                //6 - If the number could slot in the cell, register this possibility and save its coordinates
                if(!fuzzy_solution_found)
                {
                    suitable_cells++;
                    suitable_x=x+offset_x;
                    suitable_y=y+offset_y;
                }
            }
        }
        //7 - If the number can only slot in one cell:
        if(suitable_cells == 1)
        {
            //a. Write the number in the cell.
            sudoku[suitable_x][suitable_y][0] = i;

            //b. Update the missing array
            missing[i]=0;

            //c. Write the number on screen.
            cmdDrawOneNumber(suitable_x, suitable_y, i);

            //d. Register that a new number has been sloted and return. Only one solution can be found at a time with this method
            sloted_qty++;
            return sloted_qty;
        }
    }
    return sloted_qty;
}
