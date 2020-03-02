#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 5
#define COLS 5
#define MIN_BOMBS 6
#define MAX_BOMBS 8
#define GRID_SIZE 3

int board[ROWS][COLS];
int revealed[ROWS][COLS];
long points = 1;

int reveal() {
    int y;
    int x;

    getyx(stdscr, y, x);

    int board_y = y / GRID_SIZE;
    int board_x = x / GRID_SIZE;
    int value = board[board_y][board_x];

    if (value == 0) { // Bomba
        return 0;
    } else if (!revealed[board_y][board_x]) { // Casilla sin revelar
        mvaddch(y, x, value + 48);
        move(y, x);
        revealed[board_y][board_x] = 1;
        points *= value;
        return value;
    }
    
    // Casilla ya revelada
    return -1;
}

void move_cur(int y, int x) {
    int new_y;
    int new_x;

    getyx(stdscr, new_y, new_x);
    new_y += y * GRID_SIZE;
    new_x += x * GRID_SIZE;

    if (new_y >= 0 && new_y < ROWS * GRID_SIZE && new_x >= 0 && new_x < COLS * GRID_SIZE) {
        move(new_y, new_x);
    }
}

int main() {
    int ch;

    // Random seed initialization
    srand(time(NULL));

    // ncurses initialization
    init_pair(1, COLOR_RED, COLOR_BLACK);
    initscr();
    noecho();
    keypad(stdscr, TRUE);

    // Board initialization
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            revealed[i][j] = 0;
            board[i][j] = (rand() % 3) + 1;  // Random numbers between 1 and 3 (both included)
        }
    }

    // Bomb placing
    int diff_bombs = MAX_BOMBS - MIN_BOMBS;
    int remaining_bombs = diff_bombs == 0 ? MAX_BOMBS : (rand() % (MAX_BOMBS - MIN_BOMBS)) + MIN_BOMBS;
    int remaining_non_bombs = ROWS * COLS - remaining_bombs;

    while (remaining_bombs) {
        int r_row = rand() % ROWS;
        int r_col = rand() % COLS;
        if (board[r_row][r_col] != 0) {
            board[r_row][r_col] = 0;
            remaining_bombs--;
        }
    }

    int row_points[ROWS];
    int row_bombs[ROWS];
    int col_points[COLS];
    int col_bombs[ROWS];

    for (int i = 0; i < ROWS; i++) {
        row_bombs[i] = 0;
        row_points[i] = 0;
    }

    for (int j = 0; j < COLS; j++) {
        col_bombs[j] = 0;
        col_points[j] = 0;
    }

    for (int i = 0; i < ROWS; i++) {
        move(i * GRID_SIZE, 0);
        for (int j = 0; j < COLS; j++) {
            int val = board[i][j];
            row_points[i] += val;
            col_points[j] += val;
            if (!val) {
                row_bombs[i]++;
                col_bombs[j]++;
            }
            addch('.');
            for (int k = 1; k < GRID_SIZE; k++) {
                addch(' ');
            }
        }
        // Display aditional info for the row
        mvprintw(i * GRID_SIZE, COLS * GRID_SIZE, " %2d %d", row_points[i], row_bombs[i]);
    }

    for (int j = 0; j < COLS; j++) {
        mvprintw(ROWS * GRID_SIZE, j * GRID_SIZE, "%d", col_points[j]);
        mvprintw(ROWS * GRID_SIZE + 1, j * GRID_SIZE, "%d", col_bombs[j]);
    }

    move(0, 0);

    // Rendering
    refresh();
    
    int lose = 0;
    while (remaining_non_bombs && !lose && (ch = getch()) != 'q') {
        switch (ch) {
            case KEY_LEFT:
                move_cur(0, -1);
                break;
            case KEY_RIGHT:
                move_cur(0, 1);
                break;
            case KEY_UP:
                move_cur(-1, 0);
                break;
            case KEY_DOWN:
                move_cur(1, 0);
                break;
            case '\n':
                switch (reveal()) {
                    case -1:
                        break;
                    case 0:
                        lose = 1;
                        break;
                    default:
                        remaining_non_bombs--;
                }
        }
    }
    
    if (!lose) mvprintw(4, 4, "YOU WIN!");
    else mvprintw(4, 4, "YOU LOSE...");
    
    if (ch != 'q') {
        getch();
    }
    endwin();
    return 0;
}

