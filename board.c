#include "board.h"

/* Incremental string data. */
struct string_data {
  int color;                       /* Color of string, BLACK or WHITE */
  int size;                        /* Number of stones in string. */
  int origin;                      /* Coordinates of "origin", i.e. */
                                   /* "upper left" stone. */
  int liberties;                   /* Number of liberties. */
  int neighbors;                   /* Number of neighbor strings */
  int mark;                        /* General purpose mark. */
};

struct string_liberties_data {
  int list[MAX_LIBERTIES];         /* Coordinates of liberties. */
};

struct string_neighbors_data {
  int list[MAXCHAIN];              /* List of neighbor string numbers. */
};

/* Main array of string information. */
static struct string_data string[MAX_STRINGS];
static struct string_liberties_data string_libs[MAX_STRINGS];
static struct string_neighbors_data string_neighbors[MAX_STRINGS];

#define LIBERTY(pos) (board[pos] == EMPTY)
#define LIBERTIES(pos) (string[string_number[pos]].liberties)


static int delta_pos[4] = {1, -1, NS, -NS};

int board_size = DEFAULT_BOARD_SIZE;
Intersection board[BOARDSIZE];
int board_ko_pos;
int black_captured;
int white_captured;
float komi;


void clear_board(void) {
    int k;

    memset(board, 0, sizeof(board));
    for (k = 0; k < BOARDSIZE; k++) {
        if (!ON_BOARD2(I(k), J(k)))
            board[k] = GRAY;
    }

    board_ko_pos = NO_MOVE;
    black_captured = 0;
    white_captured = 0;
}

int board_empty(void) {
    int pos;
    for (pos = 0; pos < BOARDSIZE; pos++)
        if (board[pos] == BLACK || board[pos] == WHITE)
            return 0;
    return 1;
}

void add_stone(int pos, int color) {
    board[pos] = color;
}

void remove_stone(int pos) {
    board[pos] = EMPTY;
}

void play_move(int pos, int color) {
}

int stones_on_board(int color) {
}

int get_string(int i, int j, int *stonei, int *stonej){
}

int is_legal(int pos, int color) {
    /* Pass is always legal */
    if (pos == PASS_MOVE)
        return 1;

    if (!ON_BOARD1(pos))
        return 0;
    if (board[pos] != EMPTY)
        return 0;

    /* The location must not be the ko point */
    if (pos == board_ko_pos)
        if (board[WEST(pos)] == OTHER_COLOR(color)
            || board[EAST(pos)] == OTHER_COLOR(color))
            return 0;

    if (is_suicide(pos, color))
        return 0;

    return 1;
}

int is_suicide(int pos, int color) {
    int k, k_pos;
    for (k = 0; k < 4; k++) {
        k_pos = pos + delta_pos;
        if (LIBERTY(k_pos)
            || (ON_BOARD(k_pos)
                && ((board[k_pos] == color) ^ (LIBERITIES(k_pos) == 1))))
            return 0;
    }

    return 1;
}

int get_final_status(int i, int j) {
}

void set_final_status(int i, int j, int status) {
}

void compute_final_status(void) {
}

float compute_final_score(float komi) {
}

void showboard(void) {
}
