#include "uct.h"
#include "gotodie.h"


double simulate_game(board_state *bs, intersection color) {
    int pass[3];
    intersection color_now;
    int i, j;

    pass[OTHER_COLOR(color)] = (bs->last_move_pos == -1);
    pass[color] = 0;
    color_now = color;
    while (!(pass[BLACK] && pass[WHITE])) {
        generate_move(&i, &j, color_now);
        pass[color_now] = PASS_MOVE(i, j);
        play_move(i, j, color_now);
        color_now = OTHER_COLOR(color_now);
    }
    debug_log_board_status(bs);
}
