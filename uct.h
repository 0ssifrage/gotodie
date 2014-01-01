#ifndef _UCT_H_
#define _UCT_H_

#include "board.h"

#define MAX_SIM_NUM 100


typedef struct node {
    int wins;
    int visits;
    int pos;
    double uct_value;
    struct node *child, *sibling;
} uct_node;

double simulate_game(board_status *bs, intersection color);
int uct_search(board_status *bs, intersection color);


#endif
