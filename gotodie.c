#include <stdio.h>
#include <string.h>
#include "gotodie.h"
#include "board.h"

void init_gotodie(void)
{
    clear_board();
}

/* Generate a move. */
void generate_move(int *i, int *j, int color)
{
    int ai, aj, bi, bj;
    int k;
    int found = 0;

    for (ai = 0; ai < board_size && !found; ai++)
        for (aj = 0; aj < board_size && !found; aj++)
            if (board[POS(ai, aj)] == OTHER_COLOR(color)) {
                for (k = 0; k < 4 && !found; k++) {
                    bi = ai + deltai[k];
                    bj = aj + deltaj[k];
                    if (ON_BOARD(bi, bj) && legal_move(bi, bj, color)
                        && !suicide(bi, bj, color)) {
                        *i = bi;
                        *j = bj;
                        found = 1;
                    }
                }
            }
    if (!found) {
        *i = -1;
        *j = -1;
    }
}

/* Put free placement handicap stones on the board. We do this simply
 * by generating successive black moves.
 */
void place_free_handicap(int handicap)
{
    int k;
    int i, j;

    for (k = 0; k < handicap; k++) {
        generate_move(&i, &j, BLACK);
        play_move(i, j, BLACK);
    }
}
