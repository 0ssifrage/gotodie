#include "uct.h"
#include "board.h"

int main() {
    board_status b;
    clear_board(&b);
    simulate_game(&b);
    return 0;
}
