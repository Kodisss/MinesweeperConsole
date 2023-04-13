#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

Cell board[MAX_ROWS][MAX_COLS];
bool game_over = false;
int rows, cols, num_mines;

// initialize every cells
void initialize_board() {
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

// display the board if under 10 adds a space for format
void print_board() {
    // print collumn numbers
    printf("\n     ");
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
                purple(); //sets color to purple
                printf("F  "); // flag it with F if flagged
                resetColor();
            } else if (board[i][j].revealed) {
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
            } else {
                printf("%c  ", board[i][j].symbol); // otherwise print the symbol for no revealed
            }
        }
        printf("\n");
    }
    printf("\n");
}

bool is_valid_cell(int row, int col) {
    return row >= 0 && row < rows && col >= 0 && col < cols; // check for validity
}

// setup the mines dependant on probability and first guess
void place_mines(int inputRow, int inputCol) {
    int num_cells = rows * cols;
    bool testAroundFirstTry; // bool to test if the mine tries to place itself around the first guess
    num_mines = num_cells * MINE_PROBABILITY;

    if(num_mines == 0) num_mines = 1;

    for (int i = 0; i < num_mines; i++) {
        int row, col;

        // try to place a mine until you manage to pick a spot where there is no mine and isn't around the first guess
        do {
            row = rand() % rows;
            col = rand() % cols;
            testAroundFirstTry = col == inputCol || col == inputCol - 1 || col == inputCol + 1 || row == inputRow || row == inputRow - 1 || row == inputRow + 1;
        } while (board[row][col].has_mine || testAroundFirstTry);

        // place the mine
        board[row][col].has_mine = true;

        // tell the adjacent cell there is a mine here
        for (int j = row - 1; j <= row + 1; j++) {
            for (int k = col - 1; k <= col + 1; k++) {
                if (is_valid_cell(j, k)) {
                    board[j][k].adjacent_mines++;
                }
            }
        }
    }
}

void reveal_cell(int row, int col, int* cellsRemaining) {
    // check for validity
    if (!is_valid_cell(row, col) || board[row][col].revealed) {
        return;
    }

    // reveale the cell
    board[row][col].revealed = true;
    (*cellsRemaining)--;


    //check for game over
    if (board[row][col].has_mine) {
        game_over = true;
        return;
    }

    // reveal adjacent if there is no mine and recursive yourself to do that until a cell with a mine is found
    if (board[row][col].adjacent_mines == 0) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                reveal_cell(row + i, col + j, cellsRemaining); // recursive makes for the flood filling algorythm
            }
        }
    }
}

void flag_cell(int row, int col, int* mine_count) {
    // check for validity
    if (!is_valid_cell(row, col)) {
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

int main() {
    // variables used for the tooltip about wether a cell is already flagged or revealed
    bool flaggedTooltip = false;
    bool revealedTooltip = false;
    bool input_error = false;
    bool mine_placed = false;

    // reset random
    srand(time(NULL));

    rows = 4; cols = 4; // default board size

    // ask for the board size
    do{
        if(rows < 4 || cols < 4) printf("I said : minimum 4 \n");
        if(rows > MAX_ROWS) printf("Too many rows \n");
        if(cols > MAX_COLS) printf("Too many columns \n");
        printf("Enter number of rows (maximum %d, minimum 4): ", MAX_ROWS);
        scanf("%d", &rows);
        printf("Enter number of columns (maximum %d, minimum 4): ", MAX_COLS);
        scanf("%d", &cols);
    }while(rows < 4 || cols < 4 || rows > MAX_ROWS || cols > MAX_COLS);
    

    // initialization
    initialize_board();

    int mines_found = 0;
    int remaining_cells = rows * cols - num_mines;

    // game loop
    do{
        print_board();

        int row, col;
        char action;

        // tooltips gestion
        if (flaggedTooltip){
            printf("This cell is already flagged, you can unflag it by selecting F. \n");
            flaggedTooltip = false;
        }
        if (revealedTooltip){
            printf("This cell is already revealed, select another cell. \n");
            revealedTooltip = false;
        }
        if (input_error){
            printf("Input error, try again. \n");
            input_error = false;
        }

        // prompt to manage player inputs
        printf("Enter row and column (separated by a space) or 'F' to flag/unflag a cell: ");

        // if two inputs keeps going
        if (scanf("%d %d", &row, &col) == 2) {
            // if out of the board leave the loop
            if(row < 1 || col < 1 || row > rows || col > cols){
                input_error = true;
            }
            // if there is a bomb and the cell isn't flagged you lose
            else if (board[row-1][col-1].has_mine && !board[row][col].flagged) {
                game_over = true;
            }
            // else if it's flagged or already revealed tell the next loop to run the tooltip and go to next loop
            else if (board[row-1][col-1].flagged){
                flaggedTooltip = true;
            } else if (board[row-1][col-1].revealed){
                revealedTooltip = true;
            }
            // else just reveal the cell and count down
            else {
                if (!mine_placed){
                    place_mines(row-1, col-1);
                    mine_placed = true;
                }
                reveal_cell(row-1, col-1, &remaining_cells);
            }
        } 

        // else if only one input and the input is F enter flaging mode
        else if (scanf(" %c", &action) == 1 && (action == 'F' || action == 'f')) {
            printf("Enter row and column (separated by a space) to flag/unflag: ");
            scanf("%d %d", &row, &col);
            // if out of the board leave the loop
            if(row < 1 || col < 1 || row > 19 || col >19){
                input_error = true;
            }
            // if already reveal say it and get out
            else if (board[row-1][col-1].revealed){
                revealedTooltip = true;
            } 
            else{
                if (!mine_placed){
                    place_mines(row-1, col-1);
                    mine_placed = true;
                }
                flag_cell(row-1, col-1,&mines_found);
            }
        }

        else{
            input_error = true;
        }
    }while ((!game_over && remaining_cells > 0 && mines_found != num_mines) || !mine_placed);

    print_board();

    if (game_over) {
        printf("Game over! You hit a mine.\n");
    } else {
        printf("Congratulations, you won!\n");
    }
    return 0;
}