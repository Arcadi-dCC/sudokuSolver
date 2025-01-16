# Sudoku Solver
**2025 ADCC**

Sudoku Solver is a console application for Microsoft Windows that can solve your own sudokus.
From a statement in a `.txt` file, it finds the missing numbers in each cell until the
table is completed. The algorithm is executed directly from a `.exe` file, and the
solution is shown on the screen. Both statement and solution are also stored in a
local archive for later revisions.

![cmd_program_400p](https://github.com/user-attachments/assets/544e72a2-f76d-4a11-8213-bc86837fb7d9)\
Figure 1. The program solving a sudoku.

The application has been developed entirely with C programming language.

# Installation
Simply download and extract the compressed folder suitable to your machine according to its architecture:
 - 32-bit: `sudokuSolver_x86.zip`
 - 64-bit: `sudokuSolver_x64.zip`

> From now on, the final part of the names from folders and files, be it `_x86` or `_x64`, will be omitted.

The `src` folder contains the code of the program, which is not necessary to download as a user.

# User guide
**Step 1.** Open the `sudokuSolver` folder.

**Step 2.** Open the `statement.txt` file. An unsolved sudoku, which is provided as an example, will appear in it.

![typical sudoku](https://github.com/user-attachments/assets/3867c2b6-3d5c-437f-8fbb-0dd2f1ace006) 
![statement_in_txt](https://github.com/user-attachments/assets/2ad2b3b5-90cb-47e6-bf3d-bba861e6c86e)\
Figure 2 (left): typical sudoku statement. Figure 3 (right): the same sudoku written into `statement.txt`.



**Step 3.** Delete the old statement values and introduce the known elements of your sudoku.
> [!IMPORTANT]
> When typing a new statement, one must **comply with the next instructions**:
> - The divisory lines, meaning the `-` and `|` characters that divide the sudoku into 9 quadrants, must be respected.
> - The sudoku must occupy the upper- and left-most part of the file.
> - Nine characters fit inside each quadrant formed by the divisory lines. Each space is for one of the elements in that quadrant.
> - **The unknown values of the sudoku must be represented as spaces " ". This rule must be followed until the end of the grid in the right-most quadrants, and no more characters should be added past that point.**
> - Anything below the issued sudoku grid will be ommitted by the program.
> - The program can only solve one sudoku at a time, so adding multiple sudokus in the statement.txt file will not work.

**Step 4.** Save and close `statement.txt`.

**Step 5.** In the `sudokuSolver` folder, double-click on the `sudokuSolver.exe` executable file. The program will begin to solve your sudoku and show the solution on a cmd window as in Figure 1. Two outcomes are possible:

 - If the sudoku is solved in its entirety, the program will automatically save a copy of both the statement and the solution in a local archive. See the [**Archive**](#archive) section to know more.
 - If something goes wrong during the process, an error message will appear at the bottom describing the issue. See the [**Error messages**](#error-messages) section to know more.

**Step 6.** Once you no longer need to see the solution of your sudoku, press the enter key to exit the program.

## Archive
For each sudoku that is solved successfully, the program saves its statement and solution in a local archive so that the user can review them later on. The archive is located inside the `sudokuSolver` directory, in a folder named `archive`. If the folder does not exist prior to the program execution, it will be created anyway.

Each sudoku is stored in its own folder inside the archive. These folders are named after the date and time in which the containing sudoku was solved, with format `YYYYMMDD_HHMMSS`. For example, a sudoku that was solved on May 4<sup>th</sup>, 2025 at 15h 33m 56s will be stored in a folder named `20250504_153356`.

Two text files are saved inside each folder:
 - `statement.txt`: a copy of the issued statement.
 - `solution.txt`: the solution for the statement.

Note that the program does not check if the sudoku has been solved previosly. This means that if, for example, the same statement is solved five times, five new folders with the same statement and solution will be created in the archive.

## Error messages
If something goes wrong during the solution finding process, the sudoku will appear unsolved in the cmd window. Below, an error message will explain the issue:

 - **`There is something wrong with the statement file. Please review it.`** Some of the rules laid out in User Guide>Step 3 [LINK] have not been follow. Please, read them again and modify the formatting of statement.txt according to them.

 - **`The sudoku could not be solved.`** Usually, this happens when a statement has too few known elements for it to be solvable. Please review your statement in case that some elements are missing.
If this is not the case, your sudoku might actually be too difficult! Sudoku Solver is still in an early version and has a hard time dealing with the hardest of sudokus. Sorry for the inconvenience!

 - **`The sudoku was not archived correctly.`** Your sudoku has been solved, but no copy of it has been stored. However, you can still check the solution in the cmd window.
If the program is run again and the problem persists, but you really really want the answer to be stored, you can copy it directly from the cmd window and paste it in a `.txt` file.

# In-code modifications
For those with basic coding and compiling knowledge, there are some cosmetic elements of the C program that can be easily customized with
a few changes in the source files, which are issued inside the src folder.

Inside `file.h`, there are a few `#define` that give name to the archive folder, as well as the statement and solution text files. To change any of them:

 - **Name and path of the archive folder**
   1. Find `#define FILE_ARCHIVE_PATH`
   2. Change `".\\archive\\"` to `"whatever\\the\\path\\newname\\"`. To keep the archive in the `sudokuSolver` directory but changing the archive name, keep the relative path and just change to `".\\newname"`.
   3. Find `#define FILE_ARCHIVE_PATH_LEN`
   4. Update the length in characters of the path.
> [!NOTE]
> Consider that in C language, the `\` character must be expressed as `\\` and only counts as one character.

 - **Name of statement text file**
   1. Find `#define FILE_STATEMENT_NAME
   2. Change `"statement.txt"` to `"newname.txt"`.
> [!WARNING]
> This will change the name of the new statement files in the archive, but it will also search for a file with the same name in the parent folder when obtaining a statement at the beginning of the program! You should change that file's name too for the program to run properly.

 - **Name of solution text file**
   1. Find `#define FILE_SOLUTION_NAME
   2. Change `"solution.txt"` to `"newname.txt"`.

If after performing some of these changes and compiling the program, it crashes during runtime, it might be because the path name is too long. Please shorten the names of the directories you are using.  

# Disclaimers
Sudoku Solver is still in an early version, and struggles with the hardest of sudokus. We have tested the program with a set of sudoku statements, which have been solved manually too for comparison.
According to our tests, the program finds a solution for 91% of the statements. On the bright side, when a solution is found it always matches with the manually found one, so we assume that all solutions that the program finds may be regarded as correct.
