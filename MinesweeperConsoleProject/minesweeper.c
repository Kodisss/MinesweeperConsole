#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_SIZE 10
#define NUM_MINES 10

int board[BOARD_SIZE][BOARD_SIZE];
int revealed[BOARD_SIZE][BOARD_SIZE];
int num_mines_remaining = NUM_MINES;

int is_valid(int row, int col) {
    return (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
}

int is_mine(int row, int col) {
    return board[row][col] == -1;
}

void initialize_board(char board[][MAX_COLS], int rows, int cols) {
    int i, j;
    
    // initialize all cells to 'X'
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            board[i][j] = 'X';
        }
    }
}


void print_board() {
    printf("Mines remaining: %d\n", num_mines_remaining);
    printf("   ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
    }
    printf("\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d ", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (revealed[i][j]) {
                if (is_mine(i, j)) {
                    printf("* ");
                } else {
                    printf("%d ", board[i][j]);
                }
            } else {
                printf(". ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void place_mines() {
    for (int i = 0; i < NUM_MINES; i++) {
        int row = rand() % BOARD_SIZE;
        int col = rand() % BOARD_SIZE;
        if (board[row][col] == -1) {
            i--;
        } else {
            board[row][col] = -1;
            for (int j = row - 1; j <= row + 1; j++) {
                for (int k = col - 1; k <= col + 1; k++) {
                    if (is_valid(j, k) && board[j][k] != -1) {
                        board[j][k]++;
                    }
                }
            }
        }
    }
}

void reveal(int row, int col) {
    if (revealed[row][col]) {
        return;
    }
    revealed[row][col] = 1;
    if (is_mine(row, col)) {
        printf("Game over!\n");
        print_board();
        exit(0);
    }
    if (board[row][col] == 0) {
        for (int i = row - 1; i <= row + 1; i++) {
            for (int j = col - 1; j <= col + 1; j++) {
                if (is_valid(i, j)) {
                    reveal(i, j);
                }
            }
        }
    }
}

void play_game() {
    int row, col;
    while (num_mines_remaining > 0) {
        print_board();
        printf("Enter row and column to reveal: ");
        scanf("%d %d", &row, &col);
        if (!is_valid(row, col)) {
            printf("Invalid input, try again.\n");
        } else if (revealed[row][col]) {
            printf("That square has already been revealed, try again.\n");
        } else {
            reveal(row, col);
            if (num_mines_remaining == 0) {
                printf("You win!\n");
                print_board();
            }
        }
    }
}

int main() {
    srand(time(NULL));
    initialize_board();
    place_mines();
    play_game();
    return 0;
}
