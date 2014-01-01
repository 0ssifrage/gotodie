#include <stdio.h>
#include <string.h>
#include "gotodie.h"

void init_gotodie(void)
{
    clear_board(&main_board);
}

/* Generate a move. */
void generate_move(board_status *bs, int *i, int *j, intersection color)
{
    int move;

    get_legal_moves(bs, color);

    /* Choose one of the considered moves randomly with uniform
     * distribution. (Strictly speaking the moves with smaller 1D
     * coordinates tend to have a very slightly higher probability to be
     * chosen, but for all practical purposes we get a uniform
     * distribution.)
     */
    if (bs->legal_moves_num > 0) {
        move = bs->legal_moves[rand() % bs->legal_moves_num];
        *i = I(move);
        *j = J(move);
    } else {
        /* But pass if no move was considered. */
        *i = -1;
        *j = -1;
    }
}

/* Put free placement handicap stones on the board. We do this simply
 * by generating successive black moves.
 */
void place_free_handicap(board_status *bs, int handicap)
{
    int k;
    int i, j;

    for (k = 0; k < handicap; k++) {
        generate_move(bs, &i, &j, BLACK);
        play_move(bs, i, j, BLACK);
    }
}
