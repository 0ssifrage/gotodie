#include <stdio.h>
#include <string.h>
#include <time.h>
#include "gotodie.h"

void init_gotodie(void)
{
    clear_board(&main_board);
}

/* Generate a move. */
void generate_move(board_status *bs, int *i, int *j, intersection color)
{
    int moves[MAX_BOARDSIZE];
    int num_moves = 0;
    int move;
    int ai, aj;
    int k;

    memset(moves, 0, sizeof(moves));
    for (ai = 0; ai < board_size; ai++)
        for (aj = 0; aj < board_size; aj++) {
            /* Consider moving at (ai, aj) if it is legal and not suicide. */
            if (legal_move(bs, ai, aj, color)
                && !suicide(bs, ai, aj, color)) {
                /* Further require the move not to be suicide for the
                 * opponent...
                 */
                if (!suicide(bs, ai, aj, OTHER_COLOR(color)))
                    moves[num_moves++] = POS(ai, aj);
                else {
                    /* ...however, if the move captures at least one stone,
                     * consider it anyway.
                     */
                    for (k = 0; k < 4; k++) {
                        int bi = ai + deltai[k];
                        int bj = aj + deltaj[k];
                        if (ON_BOARD(bi, bj)
                            && bs->board[POS(bi, bj)] == OTHER_COLOR(color)) {
                            moves[num_moves++] = POS(ai, aj);
                            break;
                        }
                    }
                }
            }
        }

    /* Choose one of the considered moves randomly with uniform
     * distribution. (Strictly speaking the moves with smaller 1D
     * coordinates tend to have a very slightly higher probability to be
     * chosen, but for all practical purposes we get a uniform
     * distribution.)
     */
    if (num_moves > 0) {
        move = moves[rand() % num_moves];
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
