#include <stdio.h>
#include <string.h>
#include <time.h>
#include "gotodie.h"

void init_gotodie(void)
{
    clear_board(&main_board);
}

/* Generate a move. */
int generate_move(board_status *bs, intersection color)
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
        return move;
    } else {
        /* But pass if no move was considered. */
        return -14;
    }
}

int eat_more(board_status *bs, intersection color)
{
    intersection other_color = OTHER_COLOR(color);
    int flag[board_array_size];
    int other_color_cnt = 0;
    int si;
    float max_value, tmp;
    int min_pos;
    int k, spos, bi, bj;
    for (si = 1; si <= bs->num_of_strings; si++) {
        if (bs->string_color[si] == other_color) {
            other_color_cnt++;
            flag[si] = 1;
        }
    }

    while (other_color_cnt > 0) {
        max_value = -board_array_size;
        min_pos = POS(-1, -1);
        for (si = 1; si <= bs->num_of_strings; si++) {
            if (bs->string_color[si] == other_color && flag[si])
                tmp = bs->string_stones[si] * 10 - bs->approximate_liberty[si];
                // tmp = string_stones[si];
                if (tmp > max_value) {
                    max_value = tmp;
                    min_pos = bs->strings[si];
                }
        }
        spos = min_pos;
        do {
            for (k = 0; k < 4; k++) {
                bi = I(spos) + deltai[k];
                bj = J(spos) + deltaj[k];
                if (ON_BOARD(bi, bj) && legal_move(bs, bi, bj, color)
                    && !suicide(bs, bi, bj, color)) {
                    return POS(bi, bj);
                }
            }
            spos = bs->next_stone[spos];
        } while (spos != min_pos);
        other_color_cnt--;
        flag[si] = 0;
    }

    return POS(-1, -1);
}

/* Put free placement handicap stones on the board. We do this simply
 * by generating successive black moves.
 */
void place_free_handicap(board_status *bs, int handicap)
{
    int k;
    int i, j, pos;

    for (k = 0; k < handicap; k++) {
        pos = generate_move(bs, BLACK);
        play_move(bs, I(pos), J(pos), BLACK);
    }
}
