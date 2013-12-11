#include <stdio.h>

#define GOTODIE_NAME "GoToDie"
#define GOTODIE_VERSION "1.0"

void init_gotodie(void);
int place_fixed_handicap(int handicap);
void place_free_handicap(int handicap);

void generate_move(int *i, int *j, int color);
