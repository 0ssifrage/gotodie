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

extern intersection board[MAX_BOARDSIZE];
extern int last_move_pos;

/* Offsets for the four directly adjacent neighbors. Used for looping. */
extern int deltai[4];
extern int deltaj[4];

extern int next_stone[MAX_BOARDSIZE];
extern int father[MAX_BOARDSIZE];
extern int strings[MAX_BOARDSIZE];
intersection string_color[MAX_BOARDSIZE];
extern int string_stones[MAX_BOARDSIZE];
extern int num_of_strings;
extern int string_index[MAX_BOARDSIZE];

#define POS(i, j) ((i) * board_size + (j))
#define I(pos) ((pos) / board_size)
#define J(pos) ((pos) % board_size)

#define OTHER_COLOR(color) (WHITE + BLACK - (color))

#define PASS_MOVE(i, j) ((i) == -1 && (j) == -1)
#define ON_BOARD(i, j) \
    ((i) >= 0 && (i) < board_size && (j) >= 0 && (j) < board_size)


void debug_log(char *s);
void clear_board(void);
int board_empty(void);
int get_string(int i, int j, int *stonei, int *stonej);
int legal_move(int i, int j, intersection color);
int suicide(int i, int j, intersection color);
void play_move(int i, int j, intersection color);
void compute_final_status(void);
int get_final_status(int i, int j);
void set_final_status(int i, int j, int status);
int valid_fixed_handicap(int handicap);
void place_fixed_handicap(int handicap);
