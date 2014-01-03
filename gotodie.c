#include <stdio.h>
#include <string.h>
#include <time.h>
#include "gotodie.h"
#include "uct.h"

void init_gotodie(void)
{
    clear_board(&main_board);
}

/* -1 for more then 1 lib
 * else for the position of the only liberty
 */
static int only_lib(board_status *bs, int string_idx)
{
    int fpos, pos, lpos = -1;
    int i, ai, aj, bi, bj;
    int appr_lib, find_lib = 0;
    appr_lib = bs->approximate_liberty[string_idx];
    if (appr_lib > 4)
        return -1;
    fpos = bs->strings[string_idx];
    pos = fpos;
    do {
        ai = I(pos);
        aj = J(pos);
        for (i = 0; i < 4; i++) {
            bi = ai + deltai[i];
            bj = aj + deltaj[i];
            if (ON_BOARD(bi, bj) && bs->board[POS(bi, bj)] == EMPTY) {
                find_lib++;
                if (lpos == -1)
                    lpos = POS(bi, bj);
                else
                    if (lpos != POS(bi, bj))
                        return -1;
                if (find_lib == appr_lib)
                    return lpos;
            }
        }
        pos = bs->next_stone[pos];
    } while (pos != fpos);

    return lpos;
}

/* -1 for there is no stone to save */
static int save_stone(board_status *bs, intersection color)
{
    int si;
    int lpos;

    for (si = 1; si <= bs->num_of_strings; si++) {
        if (bs->string_color[si] == color) {
            lpos = only_lib(bs, si);
            if (lpos != -1 && is_legal_move(bs, color, lpos))
                return lpos;
        }
    }
    return -1;
}

/* -1 for there is no stone to capture */
static int capture_stone(board_status *bs, intersection color)
{
    int si;
    int lpos;

    for (si = 1; si <= bs->num_of_strings; si++) {
        if (bs->string_color[si] == OTHER_COLOR(color)) {
            lpos = only_lib(bs, si);
            if (lpos != -1 && is_legal_move(bs, color, lpos))
                return lpos;
        }
    }
    return -1;
}

static intersection get_turn(board_status *bs, int k,
    int di, int dj, int pos) {
    int tmp;
    if (k & 1)
        di = 2 - di;
    if ((k >> 1) & 1)
        dj = 2 - dj;
    if ((k >> 2) & 1) {
        tmp = di;
        di = dj;
        dj = tmp;
    }
    return bs->board[pos+POS(di, dj)];
}

static int match_pattern(board_status *bs, intersection color,
    int zpos)
{
    intersection f[3][3];
    int k;
    if (bs->board[zpos+14] != EMPTY)
        return 0;
    for (k = 0; k <= 7; k++) {

        // FILE *debug_file;
        // debug_file = fopen("debug.log", "a");
        // fprintf(debug_file, "%d\n", k);
        // int ii, jj;
        // for (ii = 0; ii <= 2; ii++) {
        //     for (jj = 0; jj <= 2; jj++) {
        //         f[ii][jj] = get_turn(bs, k, ii, jj, zpos);
        //         fprintf(debug_file, "%3d ", f[ii][jj]);
        //     }
        //     fprintf(debug_file, "\n");
        // }
        // fclose(debug_file);


        f[0][0] = get_turn(bs, k, 0, 0, zpos);
        if (f[0][0] != EMPTY) {
            f[0][1] = get_turn(bs, k, 0, 1, zpos);
            if (f[0][1] == OTHER_COLOR(f[0][0])) {
                f[1][0] = get_turn(bs, k, 1, 0, zpos);
                if (f[1][0] == f[0][1]) {
                    f[1][2] = get_turn(bs, k, 1, 2, zpos);
                    f[2][1] = get_turn(bs, k, 2, 1, zpos);
                    if (f[1][2] == f[0][1] || f[2][1] == f[0][1])
                        continue;
                    else
                        return 1; // Cut1
                } else if (f[1][0] == EMPTY) {
                    f[1][2] = get_turn(bs, k, 1, 2, zpos);
                    if (f[1][2] != EMPTY)
                        continue;
                    f[0][2] = get_turn(bs, k, 0, 2, zpos);
                    if (f[0][2] == f[0][0])
                        return 2; // Hane1
                    f[2][1] = get_turn(bs, k, 2, 1, zpos);
                    if (f[2][1] != EMPTY)
                        continue;
                    if (f[0][2] == f[0][1] && color == f[0][0])
                        return 3; // Hane4
                } else {
                    f[1][2] = get_turn(bs, k, 1, 2, zpos);
                    f[2][1] = get_turn(bs, k, 2, 1, zpos);
                    if (f[1][2] == EMPTY && f[2][1] == EMPTY)
                        return 4; // Hane2 && Hane3
                }
            }
        } else {
            f[1][0] = get_turn(bs, k, 1, 0, zpos);
            f[2][0] = get_turn(bs, k, 2, 0, zpos);
            if (f[1][0] != EMPTY || f[2][0] != EMPTY)
                continue;
            f[0][1] = get_turn(bs, k, 0, 1, zpos);
            if (f[0][1] == EMPTY)
                continue;
            f[2][1] = get_turn(bs, k, 2, 1, zpos);
            if (f[0][1] != f[2][1])
                continue;
            f[1][2] = get_turn(bs, k, 2, 1, zpos);
            if (f[1][2] == OTHER_COLOR(f[0][1]))
                return 5;
        }
    }
    return 0;
}

static int pattern(board_status *bs, intersection color)
{
    int lpos, res;
    int li, lj, di, dj;
    lpos = bs->last_move_pos;
    li = I(lpos);
    lj = J(lpos);
    for (di = -2; di <= 0; di++)
        for (dj = -2; dj <= 0; dj++) {
            if (ON_BOARD(li+di, lj+dj) && ON_BOARD(li+di+2, lj+dj+2)) {
                res = match_pattern(bs, color, POS(li+di, lj+dj));
                // debug_log_int(res);
                if (res)
                    return POS(li+di+1, lj+dj+1);
            }
        }
    return -1;
}

/* Generate a random move. */
int generate_random_move(board_status *bs, intersection color)
{
    int move, pos;
    // pos = save_stone(bs, color);
    // if (pos != -1)
    //     return pos;

    // pos = pattern(bs, color);
    // if (pos != -1)
    //     return pos;

    // pos = capture_stone(bs, color);
    // if (pos != -1)
    //     return pos;

    get_legal_moves(bs, color);

    /* Choose one of the considered moves randomly with uniform
     * distribution. (Strictly speaking the moves with smaller 1D
     * coordinates tend to have a very slightly higher probability to be
     * chosen, but for all practical purposes we get a uniform
     * distribution.)
     */
    if (bs->legal_moves_num > 0) {
        move = bs->legal_moves[rand() % bs->legal_moves_num];
        return move;
    } else {
        /* But pass if no move was considered. */
        return -14;
    }
}

/* Generate a move. */
int generate_move(board_status *bs, intersection color)
{
    int pos;
    pos = uct_search(bs, color);
    return pos;
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
