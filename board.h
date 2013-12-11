/* ================================================================ *
 *                         One-dimensional board                    *
 * ================================================================ */

#define DEFAULT_BOARD_SIZE 13
#define MAX_BOARD 13
#define MIN_BOARD 13

#define MAX_STRINGS (2 * MAX_BOARD * MAX_BOARD / 3)
#define MAXCHAIN 160

#define BOARDSIZE ((MAX_BOARD + 2) * (MAX_BOARD + 1) + 1)
#define BOARDMIN  (MAX_BOARD + 2)
#define BOARDMAX ((MAX_BOARD + 1) * (MAX_BOARD + 1))
#define POS(i, j) ((MAX_BOARD + 2) + (i) * (MAX_BOARD + 1) + (j))
#define I(POS) ((pos) / (MAX_BOARD + 1) - 1)
#define J(POS) ((pos) % (MAX_BOARD + 1) - 1)
#define DELTA(di, dj) ((di) * (MAX_BOARD + 1) + (dj))
#define PASS_MOVE 0
#define NO_MOVE 0
#define NS (MAX_BOARD + 1)
#define WE 1
/*
#define SOUTH(pos) ((pos) + NS)
#define WEST(pos) ((pos) - 1)
#define NORTH(pos) ((pos) - NS)
#define EAST(pos) ((pos) + 1)
#define SW(pos) ((pos) + NS - 1)
#define NW(pos) ((pos) - NS - 1)
#define NE(pos) ((pos) - NS + 1)
#define SE(pos) ((pos) + NS + 1)
#define SS(pos) ((pos) + 2 * NS)
#define WW(pos) ((pos) - 2)
#define NN(pos) ((pos) - 2 * NS)
#define EE(pos) ((pos) + 2)
*/

#define BOARD(i, j) board[POS(i, j)]

#define ON_BOARD2(i, j) ((i)>=0 && (j)>=0 && (i)<board_size && (j)<board_size)
#define ON_BOARD1(pos) (((pos)>=0 && (pos)<BOARDSIZE) && board[pos]!=GRAY)
#define ON_BOARD(pos) (board[pos] != GRAY)


enum status {
    UNKNOWN,
    ALIVE,
    DEAD,
    SEKI,
    BLACK_TERRITORY,
    WHITE_TERRITORY,
    DAME
};

/* Colors and komaster states */
enum colors {
    EMPTY,
    BLACK,
    WHITE,
    GRAY
};

#define COLOR_NAMES \
    "empty", \
    "black", \
    "white", \
    "gray"

#define OTHER_COLOR(color) (BLACK+WHITE-(color))
#define IS_STONE(arg) ((arg) == BLACK || (arg) == WHITE)

typedef unsigned char Intersection;

/* ================================================================ */
/*                         global variables                         */
/* ================================================================ */

extern int board_size;
extern Intersection board[BOARDSIZE];
extern int board_ko_pos;
extern int black_captured;
extern int white_captured;
extern float komi;


/* This struct holds the internal board state. */
struct board_state {
    int board_size;
    Intersection board[BOARDSIZE];
    int black_captured;
    int white_captured;
    float komi;
};

/* ================================================================ */
/*                        board.c functions                         */
/* ================================================================ */

void clear_board(void);
int board_empty(void);
void add_stone(int pos, int color);
void remove_stone(int pos);
void play_move(int pos, int color);
int stones_on_board(int color);

int is_legal(int pos, int color);

int get_string(int i, int j, int *stonei, int *stonej);

int get_final_status(int i, int j);
void set_final_status(int i, int j, int status);
void compute_final_status(void);
float compute_final_score(float komi);

void showboard(void);
