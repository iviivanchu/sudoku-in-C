#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

//PRIVATE VARIABLES.
int sudoku[9][9] = {0}; //The main board of the sudoku.

//Struct where I stores the positon where I want to change.
struct box_info {
  int row;
  int column;
  int number;
} box_info;

int jugar = 1; //Variable that indicates if the program is runing or not.

//PRIVATE FUNCTIONS.
void cleanBOARD(void) { //Basic function to clear all the sudoku[9][9].

  //I go through all the sudoku.
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j  < 9; j++) {
      sudoku[i][j] = 0;
    }
  }
}

bool findHOLE (int *X, int *Y) { //Function to find empty spaces in the sudoku game.

  int i = 0, j = 0;
    
  while (i < 9) { //If i is > 9 it means that I go through all the sudoku.
    if (sudoku[i][j] == 0) { //If I find a empty position I brak the bucle.
      //I modify the pointer to the variable.
      *X = i;
      *Y = j;
      return false;
    }
    else {
      if (j < 8) j++;
      else {
	j = 0;
	i++;
      }
    }
  }
  return true;
}

void *printSUDOKU(void *parameters) { //Function to print the sudoku table.

  for (int y = 0; y < 9; y++) {
    if (y == 0) printf("    %d", y);
    else printf(" %d", y);
  }
  printf("\n    - - - - - - - - -\n");
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (j == 0) printf("%d | ", i);
      printf("%d ", sudoku[i][j]);
    }
    printf("\n");
  }
}

void *checkROW(void *parameters) { //Function to see if the number is viable on the line.

  for (int i = 0; i < 9; i++) {
    if (sudoku[box_info.row][i] == (box_info.number)) return (int*)0;
  }
  return (int*)1;
}

void *checkCOLUMN(void *parameters) { //Function to see if the number is viable on the column.

  for (int i = 0; i < 9; i++) {
    if (sudoku[i][box_info.column] == (box_info.number)) return (int*)0;
  }
  return (int*)1;
}

void *checkBOX(void *parameters) { //Function to see if the number is viable on the box(3x3).

  int rowC = (box_info.row);
  int columnC = (box_info.column);

  //I restart the row of the box that I am.
  if (rowC < 3) rowC = 0;
  else if (rowC < 6) rowC = 3;
  else rowC = 6;
  
  //I restart the column of the box that I am.
  if (columnC < 3) columnC = 0;
  else if (columnC < 6) columnC = 3;
  else columnC = 6;

  for (int i = rowC; i < (rowC + 3); i++) {
    for (int j = columnC; j < (columnC + 3); j++) {
      if (sudoku[i][j] == (box_info.number)) return (int*)0;
    }
  }
  return (int*)1;
}

int checkPOSITION (struct box_info *parameters) { //Function to see that the number is viable in an exact position on the board.

  pthread_t t_ROW, t_COLUMN, t_BOX; //Threads for each function.
  void *ROW, *COLUMN, *BOX; //Thread output.

  //The first thing I do is create 3 threads for each of the checks I have to do (row column and box).
  if (pthread_create(&t_ROW, NULL, &checkROW, parameters)) return 0;
  pthread_join(t_ROW, &ROW);

  if (pthread_create(&t_COLUMN, NULL, &checkCOLUMN, parameters)) return 0;
  pthread_join(t_COLUMN, &COLUMN);

  if (pthread_create(&t_BOX, NULL, &checkBOX, parameters)) return 0;
  pthread_join(t_ROW, &BOX);

  //If the verification is successful, they will return 1, which I will save in the variables (ROW, COLUMN & BOX).
  //Next I check that the 3 outputs of the threads are 1, which indicates that it has been successful.
  if (((int*)ROW) && ((int*)COLUMN) && ((int*)BOX)) return 1;
  else return 0;
}

void inputROW(void) { //Function to read the line.

  int row;

  printf("ROW: ");

  //I wait for the entry to be a number and only a number.
  while (scanf("%d", &row) != 1) {
    printf("ROW: ");
    while (getchar() != '\n');
  }

  //I add the element in the structure.
  if (row <= 8) (box_info.row) = row;
  else inputROW();
}

void inputCOLUMN(void) { //Function to read the column.

  int column;

  printf("COLUMN: ");

  //I wait for the entry to be a number and only a number.
  while (scanf("%d", &column) != 1) {
    printf("COLUMN: ");
    while (getchar() != '\n');
  }

  //I add the element in the structure.
  if (column <= 8) (box_info.column) = column;
  else inputCOLUMN();
}

void inputNUMBER(void) { //Function to read the number.

  int number;

  printf("NUMBER: ");

  //I wait for the entry to be a number and only a number.
  while (scanf("%d", &number) != 1) {
    printf("NUMBER: ");
    while (getchar() != '\n');
  }

  //I add the element in the structure.
  if (number <= 8) (box_info.number) = number;
  else inputNUMBER();
}

void startGAME(void) { //Function in charge of reading where you want to place the number and manages all possible cases.

  int Y, X; //For the findHOLE function.
  pthread_t t_PRINT; //Thread for print function.
  
  printf("\n");
  printf("- - PUT THE DATA BELOW - -\n");

  //I wait for the structure where I save the data to fill in.
  inputROW();
  inputCOLUMN();
  inputNUMBER();

  //I check if I can put a number in that position.
  if (sudoku[box_info.row][box_info.column] != 0) {
    printf("\nERROR: SLOT FULL\n\n");
    startGAME();
  }
  //Finally I check if it is feasible to place the number in that position, check row, column and box.
  else {
    if (checkPOSITION(&box_info)) {
      sudoku[box_info.row][box_info.column] = box_info.number;
      if (findHOLE(&X, &Y)) { //If thre aren't more empty spaces that means that the sudoku is solved.
	printf("\n");
	pthread_create(&t_PRINT, NULL, &printSUDOKU, NULL);
	pthread_join(t_PRINT, NULL);
	jugar = 0;
      }
    }
    else {
      printf("\nERROR: THIS NUMBER CAN NOT BE HERE\n\n");
      startGAME();
    }
  }
}

int solveSUDOKU() { //Function in charge of solving the sudoku puzzle using the backtracking algorithm.

  int X = 0, Y = 0, i = 1; //Variables where I stored where is the empty slot.

  if (findHOLE(&X, &Y)) return 1; //Find a empty position in the sudoku.

  //Using backtracking I try to solve the sudoku puzzle for each number in each position.
  if (sudoku[X][Y] == 0) { //If I am in a empty position.
    while (i < 10) {
      box_info.number = i;
      box_info.row = X;
      box_info.column = Y;
      if (checkPOSITION(&box_info)) {
	sudoku[X][Y] = i;
	if (solveSUDOKU()) return 1;
	sudoku[X][Y] = 0;     
      }
      i++;
    }
  }
  return 0;
}

void randomize(void) { //Function that is responsible for generating a sudoku puzzle with random numbers on the board.

  int h = rand() % 21 + 50, i = 0;
  int count = 0, counter = 1;
  
  //First I clean the sudoku game.
  cleanBOARD();
  
  //I solve the sudoku game.
  solveSUDOKU();

  //I erase random numbers from the Sudoku puzzle.
  while (i < h) {
    box_info.row = rand() % 9;
    box_info.column = rand() % 9;
    box_info.number = rand() % 9;
    sudoku[box_info.row][box_info.column] = 0;
    i++;
  } 
}

void *mainMENU(void *parameters) { //Function that is responsible for managing the menu.

  int inp; //Where I store the input.
  pthread_t t_PRINT, t_MENU;

  //Here I print the sudoku board, I wait for an option, and I make sure that I get a valid option.
  printf("\n");
  printf("- - - - - - - - - - - - -\n\n");
  pthread_create(&t_PRINT, NULL, &printSUDOKU, NULL);
  pthread_join(t_PRINT, NULL);

  printf("\n");
  printf("(1) -> RESOLVE THE SUDOKU\n");
  printf("(2) -> ENTER A NUMBER\n");
  printf("(3) -> NEXT SUDOKU\n");
  printf("(4) -> EXIT\n\n");
  printf("ENTER AN OPTION -> ");

  while (scanf("%d", &inp) != 1) {
    printf("ENTER AN OPTION -> ");
    while (getchar() != '\n');
  }

  while (inp > 4) {
    printf("ENTER AN OPTION -> ");
    while (scanf("%d", &inp) != 1) {
      printf("ENTER AN OPTION -> ");
      while (getchar() != '\n');
    }
  }

  //I decide what to do depending on the option.
  if (inp == 2) startGAME();
  else if (inp == 1) {
    solveSUDOKU();
    printf("\n");
    pthread_create(&t_PRINT, NULL, &printSUDOKU, NULL);
    pthread_join(t_PRINT, NULL);
    jugar = 0;
  }
  else if (inp == 3) {
    srand(time(0));
    randomize();
    pthread_create(&t_MENU, NULL, &mainMENU, NULL);
    pthread_join(t_MENU, NULL);
  }
  else if (inp == 4) jugar = 0;
}

void main(void) {

  pthread_t t_MENU;
  //srand allows me to generate new random numbers every time I generate the sudoku puzzle.
  srand(time(0));

  printf("- - - - - - - - - - - - - \n");
  printf("    WELCOME TO SUDOKU  ");
  //The first thing I do is make a random sudoku board, so that each game is different.
  randomize();

  //As long as the play variable is 1, the menu will continue to run.
  while (jugar) {
    pthread_create(&t_MENU, NULL, &mainMENU, NULL);
    pthread_join(t_MENU, NULL);
  }
}
