#include "board.h"

#define GOTODIE_NAME "GoToDie"
#define GOTODIE_VERSION "1.0"

void init_gotodie(void);
void generate_move(int *i, int *j, intersection color);
void place_free_handicap(int handicap);
