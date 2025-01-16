#include "file.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>

//Reads the initial numbers of the sudoku found in file "statement.txt" and drops them in the first layer of a 9x9 matrix in uint8 format.
//Unknown values are zeroes.
//Returns OK if successful, NOK if the statement has missing or unended rows (bad formatting). 
uint8 fileGetStatement(uint8 row, uint8 col, uint8 dep, uint8 statement_m[row][col][dep])
{
    FILE* statement_f = fopen(FILE_STATEMENT_NAME, "r");
    if (statement_f == NULL)
    {
        return FILE_NOK;
    }

    uint8 x=0;
    uint8 y=0;
    uint8 div_found_x=0;
    uint8 div_found_y=0;

    char statement_row[30];

    for(y=0; y<11; y++)
    {
        x=0;
        div_found_x=0;

        fgets(statement_row, 30, statement_f);
        if(statement_row[0] == '-')
        {
            div_found_y++;
            continue;
        }

        while(statement_row[x] != '\n')
        {
            switch(statement_row[x])
            {
            case('|'):
                {
                    div_found_x++;
                    break;
                }
            case(' '):
                {
                    statement_m[x-div_found_x][y-div_found_y][0] = 0;
                    break;
                }
            default:
                {
                    statement_m[x-div_found_x][y-div_found_y][0]=statement_row[x] - '0';
                    break;
                }
            }
            x++;
        }
        if(x!=11)
        {
            fclose(statement_f);
            return FILE_NOK; //Statement has bad formatting in some row.
        }
    }

    fclose(statement_f);
    return FILE_OK;
}

//Creates a new directory with the designated name. Return values:
//FILE_OK         | successful
//FILE_DIR_EXISTS | folder already exists
//FILE_PATH       | incorrect path (does not create intermediate folders if they do not exist)
//FILE_NOK        | unknown error
uint8 createDirectory(char* dir_name)
{
         LPCSTR dir_name_win = dir_name;
         if(CreateDirectoryA(dir_name_win, NULL)) //This function returns 0 if error, !=0 if OK
         {
             return FILE_OK;
         }
         else
         {
             switch(GetLastError())
             {
                 case ERROR_ALREADY_EXISTS: return FILE_DIR_EXISTS;
                 case ERROR_PATH_NOT_FOUND: return FILE_PATH;
                 default: return FILE_NOK;
             }
         }
}

//Returns a string containing a relative path inside archive folder with name equivalent to date and time of
//execution with format YYYYMMDD_HHMMSS. It does not create the folder.
//returns FILE_OK if successful, FILE_NOK if directory string size does not match DIR_STR_SIZE.
uint8 fileDateTimeStr(char* dir_str, uint8 dir_str_size)
{
     if (dir_str_size != FILE_DIR_STR_SIZE) //String length check
     {
         return FILE_NOK;
     }
     
     time_t epoch = time(NULL);
     struct tm *tm = localtime(&epoch);
     
     //inside .\FILE_ARCHIVE_PATH\ folder
     strcpy(dir_str, FILE_ARCHIVE_PATH);
     
     //YYYY
     dir_str[FILE_ARCHIVE_PATH_LEN] = (1900 + tm->tm_year) / 1000 + '0'; //Millenium. tm_year 0 equals to gregorian 1900 AD
     dir_str[FILE_ARCHIVE_PATH_LEN+1] = (tm->tm_year - 100) / 100 + '0'; //Century
     dir_str[FILE_ARCHIVE_PATH_LEN+2] = (tm->tm_year % 100) / 10 + '0'; //Decade
     dir_str[FILE_ARCHIVE_PATH_LEN+3] = tm->tm_year % 10 + '0'; //Unit
     
     //MM
     dir_str[FILE_ARCHIVE_PATH_LEN+4] =  (tm->tm_mon + 1) / 10 + '0'; //Decade
     dir_str[FILE_ARCHIVE_PATH_LEN+5] =  (tm->tm_mon + 1) % 10 + '0'; //Unit
     
     //DD
     dir_str[FILE_ARCHIVE_PATH_LEN+6] =  tm->tm_mday / 10 + '0'; //Decade
     dir_str[FILE_ARCHIVE_PATH_LEN+7] =  tm->tm_mday % 10 + '0'; //Unit
     
     dir_str[FILE_ARCHIVE_PATH_LEN+8] = '_';
     
     //HH
     dir_str[FILE_ARCHIVE_PATH_LEN+9] =  tm->tm_hour / 10 + '0'; //Decade
     dir_str[FILE_ARCHIVE_PATH_LEN+10] = tm->tm_hour % 10 + '0'; //Unit
     
     //MM
     dir_str[FILE_ARCHIVE_PATH_LEN+11] = tm->tm_min / 10 + '0'; //Decade
     dir_str[FILE_ARCHIVE_PATH_LEN+12] = tm->tm_min % 10 + '0'; //Unit
     
     //SS
     dir_str[FILE_ARCHIVE_PATH_LEN+13] = tm->tm_sec / 10 + '0'; //Decade
     dir_str[FILE_ARCHIVE_PATH_LEN+14] = tm->tm_sec % 10 + '0'; //Unit

     //end of string
     dir_str[FILE_ARCHIVE_PATH_LEN+15] = 0; //equal to '\0'
     
     return FILE_OK;
}

//Copies the statement file into the archive, inside the folder with date and time of program execution.
//Return values:
// FILE_OK         | Successful.
// FILE_STATEMENT  | Original file could not be opened.
// FILE_COPY       | Copy file could not be created.
uint8 fileCopyStatement(const char* dir_str)
{
    FILE* statement_f = fopen(FILE_STATEMENT_NAME, "r");
    if (statement_f == NULL)
    {
        return FILE_STATEMENT;
    }
    
    //1 - Create string with the path to the destination directory, as well as the file name.
    char dir_file_str[FILE_DIR_STR_SIZE];
    strcpy(dir_file_str, dir_str);
    strcat(dir_file_str, "\\");
    strcat(dir_file_str, FILE_STATEMENT_NAME);
    
    //2 - Create the copy file.
    FILE* copy_f = fopen((const char*)dir_file_str, "a");
    if (copy_f == NULL)
    {
        return FILE_COPY;
    }
    
    //3 - Write the statement character by character and copy it until End Of File.
    char statement_char = getc(statement_f);
    while(statement_char != EOF)
    {
        fprintf(copy_f, "%c", statement_char);
        statement_char = getc(statement_f);
    }
    
    fclose(statement_f);
    fclose(copy_f);
    
    return FILE_OK;
}

//Creates a new solution file in the archive folder, inside the folder with date and time of program execution,
//and copies the solution found on the first layer of the sudoku matrix.
//Return values:
// FILE_OK  | Successful.
// FILE_NOK | Solution file could not be created. 
uint8 fileCopySolution(const char* dir_str, uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep])
{
    //1 - Create string with the path to the destination directory, as well as the file name.
    char dir_file_str[FILE_DIR_STR_SIZE];
    strcpy(dir_file_str, dir_str);
    strcat(dir_file_str, "\\");
    strcat(dir_file_str, FILE_SOLUTION_NAME);
    
    //2 - Create the solution file.
    FILE* solution_f = fopen((const char*)dir_file_str, "a");
    if (solution_f == NULL)
    {
        return FILE_NOK;
    }
    
    //3 - Copy the solution considering separators for each quadrant
    uint8 x=0, y=0;
    for(y=0; y<9; y++)
    {
        //Include horizontal quadrant separator
        if(y>0 && (y%3)==0)
        {
            fprintf(solution_f, "--- --- ---\n");
        }
        for(x=0;x<9;x++)
        {
            //Include vertical quadrant separator
            if(x>0 && (x%3)==0)
            {
                fprintf(solution_f, "|");
            }
            fprintf(solution_f, "%d", sudoku[x][y][0]);
        }
        fprintf(solution_f, "\n"); //Pass to the next row in the file.
    }
    
    fclose(solution_f);
    return FILE_OK;
}

//Archives the statement and the solution of the sudoku inside the archive folder.
//Each sudoku is stored in the folder with name equal to the instant of solution, in format YYYYMMDD_HHMMSS
//Returns FILE_OK if successful, FILE_NOK if error.
uint8 fileArchive(uint8 row, uint8 col, uint8 dep, uint8 sudoku[row][col][dep])
{
      char dir_name[FILE_DIR_STR_SIZE];
      
      //1 - Try to create the archive folder. If it already exists, continue.
      if(createDirectory(FILE_ARCHIVE_PATH) > FILE_DIR_EXISTS)
      {
          return FILE_NOK;
      }
      
      //2 - Find a name for the directory in which to archive the sudoku with structure YYYYMMDD_HHMMSS.
      if(fileDateTimeStr(dir_name, FILE_DIR_STR_SIZE))
      {
          return FILE_NOK;
      }
      
      //3 - Create the directory.
      if(createDirectory(dir_name) >= FILE_PATH)
      {
          return FILE_NOK;
      }
      
      //4 - Copy the statement in the directory.
      if(fileCopyStatement(dir_name))
      {
          return FILE_NOK;
      }
      
      //5 - Copy the solution from the sudoku matrix into a new solution file in the directory.
      if(fileCopySolution(dir_name, row, col, dep, sudoku))
      {
          return FILE_NOK;
      }

      return FILE_OK;
}
