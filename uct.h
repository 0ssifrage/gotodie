#ifndef _UCT_H_
#define _UCT_H_

#include "board.h"

#define MAX_SIM_NUM 20000
#define MAX_VISITS 10
/* 0.44 = sqrt(1/5) */
#define UCTK 0.44


typedef struct node {
    double wins;
    int visits;
    int pos;
    struct node *child, *sibling;
} uct_node;

double simulate_game(board_status *bs, intersection color);
int uct_search(board_status *bs, intersection color);


#endif
