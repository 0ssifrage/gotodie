#define MIN_BOARD 13
#define MAX_BOARD 13
#define BOARDSIZE 169

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

/* Board represented by a 1D array. The first board_size*board_size
 * elements are used. Vertices are indexed row by row, starting with 0
 * in the upper left corner.
 */
extern intersection board[BOARDSIZE];

/* Offsets for the four directly adjacent neighbors. Used for looping. */
extern int deltai[4];
extern int deltaj[4];

/* Macros to convert between 1D and 2D coordinates. The 2D coordinate
 * (i, j) points to row i and column j, starting with (0,0) in the
 * upper left corner.
 */
#define POS(i, j) ((i) * board_size + (j))
#define I(pos) ((pos) / board_size)
#define J(pos) ((pos) % board_size)

/* Macro to find the opposite color. */
#define OTHER_COLOR(color) (WHITE + BLACK - (color))

void clear_board(void);
int board_empty(void);
int get_string(int i, int j, int *stonei, int *stonej);
int on_board(int i, int j);
int legal_move(int i, int j, int color);
int suicide(int i, int j, int color);
void play_move(int i, int j, int color);
void compute_final_status(void);
int get_final_status(int i, int j);
void set_final_status(int i, int j, int status);
int valid_fixed_handicap(int handicap);
void place_fixed_handicap(int handicap);
