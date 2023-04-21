#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#define MAX_ROWS 20
#define MAX_COLS 20
#define MINE_PROBABILITY 0.15

// Cell struct to fill the board with
typedef struct {
    char symbol;
    int adjacent_mines;
    bool has_mine;
    bool revealed;
    bool flagged;
} Cell;

// initialize every cells
void initialize_board(Cell **board, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            board[i][j].symbol = 'X';
            board[i][j].adjacent_mines = 0;
            board[i][j].has_mine = false;
            board[i][j].revealed = false;
            board[i][j].flagged = false;
        }
    }
}

void boardSizing(int *rows, int *cols){
    // ask for the board size
    do{
        if(*rows < 4 || *cols < 4) printf("I said : minimum 4 \n");
        if(*rows > MAX_ROWS) printf("Too many rows \n");
        if(*cols > MAX_COLS) printf("Too many columns \n");
        printf("Enter number of rows (maximum %d, minimum 4): ", MAX_ROWS);
        scanf("%d", rows);
        printf("Enter number of columns (maximum %d, minimum 4): ", MAX_COLS);
        scanf("%d", cols);
    }while(*rows < 4 || *cols < 4 || *rows > MAX_ROWS || *cols > MAX_COLS);
}

void red(){
    printf("\033[0;31m");
}

void green(){
    printf("\033[0;32m");
}

void cyan(){
    printf("\033[0;36m");
}

void yellow(){
    printf("\033[0;33m");
}

void purple(){
    printf("\033[0;35m");
}

void resetColor(){
    printf("\033[0m");
}

void colorChoice(bool *inputColors){
    char colorChoice;

    // ask to use color or not
    do{
        printf("Do you want to use colors (don't if you see weird numbers everywhere instead of colored numbers) Yes [Y] or No [N] ?");
        scanf(" %c", &colorChoice);
    }while(colorChoice != 'Y' && colorChoice != 'N' && colorChoice != 'y' && colorChoice != 'n');

    if(colorChoice == 'Y' || colorChoice == 'y'){
        *inputColors = true;
    }else if(colorChoice == 'N' || colorChoice == 'n'){
        *inputColors = false;
    }
}

void colorPrinting(Cell **board, int i, int j){
    if(board[i][j].flagged){
        purple(); //sets color to purple
        printf("F  "); // flag it with F if flagged
        resetColor();
        return;
    }
    if(board[i][j].adjacent_mines == 0){
        green(); //sets color to green
        printf("%d  ", board[i][j].adjacent_mines);
        resetColor();
    } else if(board[i][j].adjacent_mines == 1){
        cyan(); //sets color to cyan
        printf("%d  ", board[i][j].adjacent_mines);
        resetColor();
    } else if(board[i][j].adjacent_mines == 2){
        yellow(); //sets color to yellow
        printf("%d  ", board[i][j].adjacent_mines);
        resetColor();
    } else{
        red(); //sets color to red
        printf("%d  ", board[i][j].adjacent_mines);
        resetColor();
    }
}

// display the board if under 10 adds a space for format
void print_board(Cell **board, int rows, int cols, bool useColors) {
    system("CLS");
    printf("\n     ");

    // print collumn numbers
    for (int i = 0; i < cols; i++) {
        if (i < 9) {
            printf("%d  ", i+1);
        } else {
            printf("%d ", i+1);
        }
    }

    // print row numbers
    printf("\n\n");
    for (int i = 0; i < rows; i++) {
        if (i < 9) {
            printf("%d    ", i+1);
        } else {
            printf("%d   ", i+1);
        }
        // complete the board
        for (int j = 0; j < cols; j++) {
            if (board[i][j].flagged) {
                if(useColors){
                    colorPrinting(board, i, j);
                }
                else{
                    printf("F  "); // flag it with F if flagged
                }
            } else if (board[i][j].revealed) {
                // use colors
                if(useColors){
                    colorPrinting(board, i, j);
                // or not
                } else{
                    printf("%d  ", board[i][j].adjacent_mines);
                }
            } else {
                printf("%c  ", board[i][j].symbol); // otherwise print the symbol for no revealed
            }
        }
        printf("\n");
    }
    printf("\n");
}

bool is_valid_cell(int rowInput, int colInputs, int maxRows, int maxCols) {
    return rowInput >= 0 && rowInput < maxRows && colInputs >= 0 && colInputs < maxCols; // check for validity
}

// setup the mines dependant on probability and first guess
void place_mines(Cell **board, int maxRows, int maxCols, int inputRow, int inputCol, int *num_mines, int *cellsRemaining) {
    int num_cells = maxRows * maxCols;
    bool testAroundFirstTry; // bool to test if the mine tries to place itself around the first guess
    int antibug;
    int upTo = 400;
    int substractMineCount = 0;
    *num_mines = num_cells * MINE_PROBABILITY;

    if(*num_mines == 0) *num_mines = 1;

    for (int i = 0; i < *num_mines; i++) {
        int row, col;
        antibug = 0;

        // try to place a mine until you manage to pick a spot where there is no mine and isn't around the first guess
        do {
            antibug++;
            row = rand() % maxRows;
            col = rand() % maxCols;
            testAroundFirstTry = (col == inputCol - 1 && row == inputRow - 1) || (col == inputCol && row == inputRow - 1) || (col == inputCol + 1 && row == inputRow - 1)
                                || (col == inputCol - 1 && row == inputRow) || (col == inputCol && row == inputRow) || (col == inputCol + 1 && row == inputRow)
                                || (col == inputCol - 1 && row == inputRow + 1) || (col == inputCol && row == inputRow + 1) || (col == inputCol + 1 && row == inputRow + 1);
            //printf("%d \n", antibug);
        } while ((board[row][col].has_mine || testAroundFirstTry) && antibug < upTo);

        if(antibug < upTo){
            // place the mine
            board[row][col].has_mine = true;

            // tell the adjacent cell there is a mine here
            for (int j = row - 1; j <= row + 1; j++) {
                for (int k = col - 1; k <= col + 1; k++) {
                    if (is_valid_cell(j, k, maxRows, maxCols)) {
                        board[j][k].adjacent_mines++;
                    }
                }
            }
        }else{
            substractMineCount++; //keeps track of the number of mines skipped
        }
    }
    //printf("%d ",*num_mines);
    *num_mines -= substractMineCount; // substracts the number of mines skipped
    *cellsRemaining = maxRows * maxCols - *num_mines;
    //printf("%d \n",*num_mines);
}

void reveal_cell(Cell **board, int maxRows, int maxCols, int row, int col, int* cellsRemaining, bool* gameOver) {
    // check for validity
    if (!is_valid_cell(row, col, maxRows, maxCols) || board[row][col].revealed || board[row][col].flagged) {
        return;
    } else if (board[row][col].has_mine) {
        *gameOver = true;
        return;
    }

    // reveal the cell
    board[row][col].revealed = true;
    (*cellsRemaining)--;

    // reveal adjacent if there is no mine and recursive yourself to do that until a cell with a mine is found
    if (board[row][col].adjacent_mines == 0) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                reveal_cell(board, maxRows, maxCols, row + i, col + j, cellsRemaining, gameOver); // recursive makes for the flood filling algorythm
            }
        }
    }
}

void flag_cell(Cell **board, int maxRows, int maxCols, int row, int col, int* mine_count) {
    // check for validity
    if (!is_valid_cell(row, col, maxRows, maxCols)) {
        return;
    }

    // if not flagged and there is a mine, you found a mine
    if (board[row][col].has_mine && !board[row][col].flagged){
        (*mine_count)++;
    }

    // if already flagged and there is a mine, you lost a mine
    if (board[row][col].has_mine && board[row][col].flagged){
        (*mine_count)--;
    }

    // flag or unflag the cell
    board[row][col].flagged = !board[row][col].flagged;
}

void toolTipsGestion(bool *flagged, bool *revealed){
    // tooltips gestion
        if (*flagged){
            printf("This cell is already flagged, you can unflag it by selecting F. \n");
            *flagged = false;
        }
        if (*revealed){
            printf("This cell is already revealed, select another cell. \n");
            *revealed = false;
        }
}

int* GetInputNumber(const char* message, int min, int max1, int max2, bool* isFlagged) {
    static int nums[2];
    char input[256];
    int success = 0;

    printf("%s", message);
    
    do {
        // check for good inputing ? 
        if (fgets(input, 256, stdin) == NULL) {
            printf("Error reading input\n");
        }

        // input gathering
        if (toupper(input[0]) == 'F') {
            *isFlagged = true;
            success = sscanf(input + 1, "%d %d", &nums[0], &nums[1]);
        } else {
            *isFlagged = false;
            success = sscanf(input, "%d %d", &nums[0], &nums[1]);
        }

        // input interpretations
        if(success == EOF){}
        else if (success == 1) {
            printf("Please enter two numbers separated by a space.\n");
        } else if (nums[0] < min || nums[0] > max1 || nums[1] < min || nums[1] > max2) {
            printf("Please enter two numbers between %d and %d and %d and %d.\n", min, max1, min, max2);
            success = 0;
        }
    } while (success != 2);
    
    return nums;
}

int main() {
    system("CLS");
    // game global variables
    bool game_over = false;
    int rows, cols, num_mines;
    bool useColors = true;

    // variables used for the tooltip about wether a cell is already flagged or revealed
    bool flaggedTooltip = false;
    bool revealedTooltip = false;
    bool mine_placed = false;

    // game win or lose variables
    int mines_found = 0;
    int remaining_cells = 0;

    // game loop variables
    int row, col;
    int* userInputs;
    bool flaggedInput;
    int checkAroundRevealed;

    // ask for colors or not
    colorChoice(&useColors);

    // reset random
    srand(time(NULL));

    rows = 4; cols = 4; // default board size

    // board size thingy
    boardSizing(&rows, &cols);

    // board table initilization
    Cell **board = malloc(rows * sizeof(Cell *));
    for (int i = 0; i < rows; i++) {
        board[i] = malloc(cols * sizeof(Cell));
    }
    
    // initialization for game loop
    initialize_board(board, rows, cols);

    // game loop
    do{
        print_board(board, rows, cols, useColors);

        checkAroundRevealed = 0;

        toolTipsGestion(&flaggedTooltip, &revealedTooltip);

        // gather player inputs
        userInputs = GetInputNumber("Enter row and column (separated by a space) to reveal \nEnter 'F' followed by row and column (separated by a space) to flag/unflag a cell: ", 1, rows, cols, &flaggedInput);
        row = userInputs[0];
        col = userInputs[1];

        // when the input is not on a flagged tile you run the "normal loop" if it is, run the "flagged loop"
        if(!flaggedInput){
            // if there is a bomb and the cell isn't flagged you lose
            if (board[row-1][col-1].has_mine && !board[row-1][col-1].flagged) {
                game_over = true;
            }
            // else if it's flagged or already revealed tell the next loop to run the tooltip and go to next loop
            else if (board[row-1][col-1].flagged){
                flaggedTooltip = true;
            } else if (board[row-1][col-1].revealed){
                for(int i = -1; i <= 1; i++){
                    for(int j = -1; j <= 1; j++){
                        if (board[row-1+i][col-1+j].flagged) checkAroundRevealed++;
                    }
                }
                //printf("%d", checkAroundRevealed);
                if(board[row-1][col-1].adjacent_mines == checkAroundRevealed){
                    //printf("entered first if ");
                    for(int i = -1; i <= 1; i++){
                        for(int j = -1; j <= 1; j++){
                            //printf("[%d;%d] ", row+i, col+j);
                            if(!board[row-1+i][col-1+j].revealed){
                                //printf("entered second if ");
                                reveal_cell(board, rows, cols, row-1+i, col-1+j, &remaining_cells, &game_over);
                            }
                        }
                    }
                } else {
                    revealedTooltip = true;
                }
            }
            // else just reveal the cell and count down
            else {
                if (!mine_placed){
                    place_mines(board, rows, cols, row-1, col-1, &num_mines, &remaining_cells);

                    mine_placed = true;
                }
                reveal_cell(board, rows, cols, row-1, col-1, &remaining_cells, &game_over);
            }
        }else{
            // if already reveal say it and get out
            if (board[row-1][col-1].revealed){
                revealedTooltip = true;
            }
            else{
                if (!mine_placed){
                    place_mines(board, rows, cols, row-1, col-1, &num_mines, &remaining_cells);
                    mine_placed = true;
                }
                flag_cell(board, rows, cols, row-1, col-1, &mines_found);
            }
        }
    }while ((!game_over && remaining_cells > 0 && mines_found != num_mines) || !mine_placed);

    print_board(board, rows, cols, useColors);

    // when out of the loop, chack for game over or not and display the appropriate message
    if (game_over) {
        printf("Game over! You hit a mine.\n");
    } else {
        printf("Congratulations, you won!\n");
    }

    // free memory from board
    for (int i = 0; i < rows; i++) {
        free(board[i]);
    }
    free(board);

    // waits for users input to close
    system("pause");

    return 0;
}