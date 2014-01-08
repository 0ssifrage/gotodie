#include "board.h"
#include "gotodie.h"
#include "uct.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


double simulate_game(board_status *bs, intersection color)
{
    int pass[3];
    intersection color_now;
    int i, j, pos, step;
    double score;

    step = 0;
    pass[OTHER_COLOR(color)] = (bs->last_move_pos == -14);
    pass[color] = 0;
    color_now = color;
    while (!(pass[BLACK] && pass[WHITE]) && step <= 200) {
        pos = generate_random_move(bs, color_now);
        pass[color_now] = (pos == -14);
        play_move(bs, I(pos), J(pos), color_now);
        color_now = OTHER_COLOR(color_now);
    }
    // debug_log_board_status(bs);
    score = get_score(bs);
    if (score > 0 && color == WHITE)
        return 1;
    if (score < 0 && color == BLACK)
        return 1;
    return 0;
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
        int k = un->pos;
        free(un);
    }
}

static int create_uct_children(board_status *bs, intersection color,
    uct_node *un)
{
    int mi;
    uct_node *now, *new_node;

    get_legal_moves2(bs, color);

    if (bs->legal_moves_num == 0)
        return 0;

    new_node = (uct_node *)malloc(sizeof(uct_node));
    init_uct_node(-1, -1, new_node);
    new_node->pos = bs->legal_moves[0];
    un->child = new_node;
    now = un->child;

    for (mi = 1; mi < bs->legal_moves_num; mi++) {
        new_node = (uct_node *)malloc(sizeof(uct_node));
        init_uct_node(-1, -1, new_node);
        new_node->pos = bs->legal_moves[mi];
        now->sibling = new_node;
        now = now->sibling;
    }
    return bs->legal_moves_num;
}

static uct_node* uct_select(uct_node *un)
{
    uct_node *now, *tmp;
    double uct_value, max_uct_value, win_rate;

    now = un->child;
    max_uct_value = -1.0;
    tmp = NULL;
    while (now != NULL) {
        if (now->visits > 0) {
            win_rate = now->wins * 1.0 / now->visits;
            uct_value = win_rate+UCTK*sqrt(log(un->visits)*1.0/now->visits);
        } else
            uct_value = 10000 + (1000 * rand());
        if (uct_value > max_uct_value) {
            max_uct_value = uct_value;
            tmp = now;
        }
        now = now->sibling;
    }
    return tmp;
}

static void update_node(double res, uct_node *un) {
    un->wins += res;
    un->visits += 1;
}

static double simulate(board_status *bs, intersection color, uct_node *un)
{
    double res;
    uct_node *next;
    int i, j;

    if (un->child == NULL && un->visits < MAX_VISITS)
        res = simulate_game(bs, color);
    else {
        if (un->child == NULL)
            create_uct_children(bs, color, un);
        next = uct_select(un);
        if (next == NULL) {
            next = (uct_node *)malloc(sizeof(uct_node));
            init_uct_node(-1, -1, next);
        }
        play_move(bs, I(next->pos), J(next->pos), color);
        color = OTHER_COLOR(color);
        res = simulate(bs, color, next);
        res = 1 - res;
    }

    update_node(1 - res, un);
    return res;
}

static uct_node* get_best_child(uct_node *uct_root)
{
    int max_visits = -1;
    uct_node *un, *bn;
    un = uct_root->child;
    bn = NULL;
    while (un != NULL) {
        // printf("%d\n", un->visits);
        if (un->visits > max_visits) {
            max_visits = un->visits;
            bn = un;
        }
        un = un->sibling;
    }
    return bn;
}

int uct_search(board_status *bs, intersection ucolor)
{
    uct_node *uct_root;
    uct_node *best_child;
    board_status uct_board;
    intersection color;
    int i, node_num, pos;

    memcpy(&uct_board, bs, sizeof(uct_board));
    color = ucolor;
    uct_root = (uct_node *)malloc(sizeof(uct_node));
    init_uct_node(-1, -1, uct_root);
    node_num = create_uct_children(&uct_board, color, uct_root);

    i = 0;
    while (i < MAX_SIM_NUM) {
        i++;
        simulate(&uct_board, color, uct_root);
        memcpy(&uct_board, bs, sizeof(uct_board));
        color = ucolor;
    }
    best_child = get_best_child(uct_root);

    if (best_child == NULL)
        pos = POS(-1,-1);
    else
        pos = best_child->pos;

    free_uct_tree(uct_root);
    return pos;
}
