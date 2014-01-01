#include <stdio.h>
#include <string.h>
#include "gotodie.h"
#include "board.h"

void init_gotodie(void)
{
    clear_board();
}

/* Generate a move. */
int generate_move(intersection color)
{
    intersection other_color = OTHER_COLOR(color);
    int flag[board_array_size];
    int other_color_cnt = 0;
    int si;
    float max_value, tmp;
    int min_pos;
    int k, spos, bi, bj;
    for (si = 1; si <= num_of_strings; si++) {
        if (string_color[si] == other_color) {
            other_color_cnt++;
            flag[si] = 1;
        }
    }

    while (other_color_cnt > 0) {
        max_value = -board_array_size;
        min_pos = POS(-1, -1);
        for (si = 1; si <= num_of_strings; si++) {
            if (string_color[si] == other_color && flag[si])
                tmp = string_stones[si] * 10 - approximate_liberty[si];
                // tmp = string_stones[si];
                if (tmp > max_value) {
                    max_value = tmp;
                    min_pos = strings[si];
                }
        }
        spos = min_pos;
        do {
            for (k = 0; k < 4; k++) {
                bi = I(spos) + deltai[k];
                bj = J(spos) + deltaj[k];
                if (ON_BOARD(bi, bj) && legal_move(bi, bj, color)
                    && !suicide(bi, bj, color)) {
                    return POS(bi, bj);
                }
            }
            spos = next_stone[spos];
        } while (spos != min_pos);
        other_color_cnt--;
        flag[si] = 0;
    }

    return POS(-1, -1);
}

/* Put free placement handicap stones on the board. We do this simply
 * by generating successive black moves.
 */
void place_free_handicap(int handicap)
{
    int k;
    int i, j, pos;

    for (k = 0; k < handicap; k++) {
        pos = generate_move(BLACK);
        play_move(I(pos), J(pos), BLACK);
    }
}
