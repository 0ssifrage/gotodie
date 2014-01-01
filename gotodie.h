#include "board.h"

#define GOTODIE_NAME "GoToDie"
#define GOTODIE_VERSION "1.0"

void init_gotodie(void);
void generate_move(board_status *bs, int *i, int *j, intersection color);
void place_free_handicap(board_status *bs, int handicap);
