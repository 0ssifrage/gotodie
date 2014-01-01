#include "board.h"
#include "gotodie.h"
#include "uct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


double simulate_game(board_status *bs, intersection color)
{
    int pass[3];
    intersection color_now;
    int i, j, pos, step;

    step = 0;
    pass[OTHER_COLOR(color)] = (bs->last_move_pos == -14);
    pass[color] = 0;
    color_now = color;
    while (!(pass[BLACK] && pass[WHITE]) && step <= 200) {
        pos = generate_move(bs, color_now);
        pass[color_now] = (pos == -14);
        play_move(bs, I(pos), J(pos), color_now);
        color_now = OTHER_COLOR(color_now);
    }
    // debug_log_board_status(bs);

    return get_score(bs);
}

static void init_uct_node(int wins, int visits, uct_node *un)
{
    un->wins = wins;
    un->visits = visits;
    un->pos = POS(-1, -1);
    un->child = NULL;
    un->sibling = NULL;
}

static void free_uct_tree(uct_node *un)
{
    if (un != NULL) {
        free_uct_tree(un->child);
        free_uct_tree(un->sibling);
        free(un);
    }
}

static void simulate(board_status *bs, intersection color)
{

}

static void create_uct_children(board_status *bs, uct_node *un)
{

}

static void get_best_child(uct_node *uct_root, uct_node *best_child)
{
    int max_visits = -1;
    uct_node *un, *bn;
    un = uct_root->child;
    while (un != NULL) {
        if (un->visits > max_visits) {
            max_visits = un->visits;
            bn = un;
        }
        un = un->sibling;
    }
    best_child = bn;
}

int uct_search(board_status *bs, intersection color)
{
    uct_node uct_root;
    uct_node *best_child;
    board_status uct_board;
    int i;

    memcpy(&uct_board, bs, sizeof(uct_board));
    init_uct_node(-1, -1, &uct_root);
    create_uct_children(&uct_board, &uct_root);

    i = 0;
    while (i < MAX_SIM_NUM) {
        i++;
        simulate(&uct_board, color);
    }
    get_best_child(&uct_root, best_child);
    free_uct_tree(&uct_root);
    return best_child->pos;
}
