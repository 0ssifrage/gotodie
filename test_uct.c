#include "uct.h"
#include "board.h"
#include "gotodie.h"


int main() {
    board_status b;
    clear_board(&b);
    simulate_game(&b, BLACK);
    return 0;
}
