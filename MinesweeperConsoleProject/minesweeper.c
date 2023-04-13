#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_ROWS 20
#define MAX_COLS 20
#define MINE_PROBABILITY 0.15

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

void print_board() {
    printf("\n    ");
    for (int i = 0; i < cols; i++) {
        printf("%d ", i);
    }
    printf("\n\n");
    for (int i = 0; i < rows; i++) {
        printf("%d   ", i);
        for (int j = 0; j < cols; j++) {
            if (board[i][j].marked) {
                printf("M ");
            } else if (board[i][j].revealed) {
                printf("%d ", board[i][j].adjacent_mines);
            } else {
                printf("%c ", board[i][j].symbol);
            }
        }
        printf("\n");
    }
    printf("\n");
}

bool is_valid_cell(int row, int col) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

void place_mines() {
    int num_cells = rows * cols;
    num_mines = num_cells * MINE_PROBABILITY;
    for (int i = 0; i < num_mines; i++) {
        int row, col;
        do {
            row = rand() % rows;
            col = rand() % cols;
        } while (board[row][col].has_mine);
        board[row][col].has_mine = true;
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
    if (!is_valid_cell(row, col) || board[row][col].revealed) {
        return;
    }
    board[row][col].revealed = true;
    if (board[row][col].has_mine) {
        game_over = true;
        return;
    }
    if (board[row][col].adjacent_mines == 0) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                reveal_cell(row + i, col + j);
            }
        }
    }
}

void mark_cell(int row, int col) {
    if (!is_valid_cell(row, col)) {
        return;
    }
    board[row][col].marked = !board[row][col].marked;
}

int main() {
    srand(time(NULL));
    printf("Enter number of rows (maximum %d): ", MAX_ROWS);
    scanf("%d", &rows);
    printf("Enter number of columns (maximum %d): ", MAX_COLS);
    scanf("%d", &cols);
    initialize_board();
    place_mines();
    int remaining_cells = rows * cols - num_mines;
    while (!game_over && remaining_cells > 0) {
        print_board();
        int row, col;
        printf("Enter row and column (separated by a space) or 'M' to mark/unmark a cell: ");
        if (scanf("%d %d", &row, &col) == 2) {
            if (board[row][col].has_mine) {
                game_over = true;
            } else {
                reveal_cell(row, col);
                remaining_cells--;
            }
        } else {
            char mark;
            scanf(" %c", &mark);
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