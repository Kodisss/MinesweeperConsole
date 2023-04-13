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
    bool marked;
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
            board[i][j].marked = false;
        }
    }
}

// display the board
void print_board() {
    printf("\n     ");
    for (int i = 0; i < cols; i++) {
        if (i < 10) {
            printf("%d  ", i);
        } else {
            printf("%d ", i);
        }
    }
    printf("\n\n");
    for (int i = 0; i < rows; i++) {
        if (i < 10) {
            printf("%d    ", i);
        } else {
            printf("%d   ", i);
        }
        for (int j = 0; j < cols; j++) {
            if (board[i][j].marked) {
                printf("M  ");
            } else if (board[i][j].revealed) {
                printf("%d  ", board[i][j].adjacent_mines);
            } else {
                printf("%c  ", board[i][j].symbol);
            }
        }
        printf("\n");
    }
    printf("\n");
}



bool is_valid_cell(int row, int col) {
    return row >= 0 && row < rows && col >= 0 && col < cols; // check for validity
}

// setup the mines dependant on probability
void place_mines() {
    int num_cells = rows * cols;
    num_mines = num_cells * MINE_PROBABILITY;

    for (int i = 0; i < num_mines; i++) {
        int row, col;

        // try to place a mine until you manage to pick a spot where there is no mine
        do {
            row = rand() % rows;
            col = rand() % cols;
        } while (board[row][col].has_mine);

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

void reveal_cell(int row, int col) {
    // check for validity
    if (!is_valid_cell(row, col) || board[row][col].revealed) {
        return;
    }

    // reveale the cell
    board[row][col].revealed = true;

    //check for game over
    if (board[row][col].has_mine) {
        game_over = true;
        return;
    }

    // reveal adjacent if there is no mine and recursive yourself to do that until a cell with a mine is found
    if (board[row][col].adjacent_mines == 0) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                reveal_cell(row + i, col + j); // recursive makes for the flood filling algorythm
            }
        }
    }
}

void mark_cell(int row, int col) {
    // check for validity
    if (!is_valid_cell(row, col)) {
        return;
    }
    // mark the cell
    board[row][col].marked = !board[row][col].marked;
}

int main() {
    // variables used for the tooltip about wether a cell is already marked or revealed
    bool markedTooltip = false;
    bool revealedTooltip = false;

    // reset random
    srand(time(NULL));

    // ask for the board size
    printf("Enter number of rows (maximum %d): ", MAX_ROWS);
    scanf("%d", &rows);
    printf("Enter number of columns (maximum %d): ", MAX_COLS);
    scanf("%d", &cols);

    // initialization
    initialize_board();
    place_mines();

    int remaining_cells = rows * cols - num_mines;

    // game loop
    while (!game_over && remaining_cells > 0) {
        print_board();

        int row, col;
        char action;

        // tooltips gestion
        if (markedTooltip){
            printf("This cell is already marked, you can unmark it by selecting M. \n");
            markedTooltip = false;
        }
        if (revealedTooltip){
            printf("This cell is already revealed, select another cell. \n");
            revealedTooltip = false;
        }

        // prompt to manage player inputs
        printf("Enter row and column (separated by a space) or 'M' to mark/unmark a cell: ");

        // if two inputs keeps going
        if (scanf("%d %d", &row, &col) == 2) {
            // if there is a bomb and the cell isn't marked you lose
            if (board[row][col].has_mine && !board[row][col].marked) {
                game_over = true;
            }

            // else if it's marked or already revealed tell the next loop to run the tooltip and go to next loop
            else if (board[row][col].marked){
                markedTooltip = true;
            } else if (board[row][col].revealed){
                revealedTooltip = true;
            }
            // else just reveal the cell and count down
            else {
                reveal_cell(row, col);
                remaining_cells--;
            }
        } 

        // else if only one input and the input is M enter marking mode
        else if (scanf(" %c", &action) == 1 && action == 'M') {
            printf("Enter row and column (separated by a space) to mark/unmark: ");
            scanf("%d %d", &row, &col);
            mark_cell(row, col);
        }
    }

    print_board();

    if (game_over) {
        printf("Game over! You hit a mine.\n");
    } else {
        printf("Congratulations, you won!\n");
    }
    return 0;
}