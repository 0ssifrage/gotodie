#ifndef _BOARD_H_
#define _BOARD_H_

#define MIN_BOARD 13
#define MAX_BOARD 13
#define MAX_BOARDSIZE 169

#define EMPTY 0
#define WHITE 1
#define BLACK 2

#define DEAD 0
#define ALIVE 1
#define SEKI 2
#define WHITE_TERRITORY 3
#define BLACK_TERRITORY 4
#define UNKNOWN 5

typedef unsigned char intersection;

extern float komi;
extern int board_size;
extern int board_array_size;

typedef struct sbs {
    intersection board[MAX_BOARDSIZE];
    int last_move_pos;
    /* Point which would be an illegal ko recapture. */
    int ko_i, ko_j, ko_pos;
    /* Stones are linked together in a circular list for each string. */
    int next_stone[MAX_BOARDSIZE];
    int father[MAX_BOARDSIZE];
    /* all strings in the board */
    int strings[MAX_BOARDSIZE];
    intersection string_color[MAX_BOARDSIZE];
    int string_stones[MAX_BOARDSIZE];
    int approximate_liberty[MAX_BOARDSIZE];
    int num_of_strings;
    /* strings[string_index[get_father(pos)]] = get_father(pos) */
    int string_index[MAX_BOARDSIZE];

    int legal_moves[MAX_BOARDSIZE];
    int legal_moves_num;

    int legal_moves2[MAX_BOARDSIZE];
    int legal_moves_num2;

    int legal[2][MAX_BOARDSIZE];

    /* Storage for final status computations. */
    int final_status[MAX_BOARDSIZE];
    int update_pos[MAX_BOARDSIZE];
} board_status;

extern board_status main_board;


/* Offsets for the four directly adjacent neighbors. Used for looping. */
extern int deltai[4];
extern int deltaj[4];

#define POS(i, j) ((i) * board_size + (j))
#define I(pos) ((pos) / board_size)
#define J(pos) ((pos) % board_size)

#define OTHER_COLOR(color) (WHITE + BLACK - (color))

#define PASS_MOVE(i, j) ((i) == -1 && (j) == -1)
#define ON_BOARD(i, j) \
    ((i) >= 0 && (i) < board_size && (j) >= 0 && (j) < board_size)


void debug_log_int(int i);
void debug_log(char *s);
void debug_log_board_status(board_status *bs);
void clear_board(board_status *bs);
int board_empty(board_status *bs);
int is_stone(board_status *bs, int pos);
int get_string(board_status *bs, int i, int j, int *stonei, int *stonej);
int legal_move(board_status *bs, int i, int j, intersection color);
int only_lib(board_status *bs, int string_idx);
int suicide(board_status *bs, int i, int j, intersection color);
void get_legal_moves(board_status *bs, intersection color);
int get_father(board_status *bs, int pos);
void play_move(board_status *bs, int i, int j, intersection color);
void compute_final_status(board_status *bs);
int get_final_status(board_status *bs, int i, int j);
void set_final_status(board_status *bs, int i, int j, int status);
double get_score(board_status *bs);
int valid_fixed_handicap(int handicap);
void place_fixed_handicap(board_status *bs, int handicap);


#endif
