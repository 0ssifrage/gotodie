#include <stdio.h>
#include <string.h>
#include "gotodie.h"
#include "board.h"

void init_gotodie(void)
{
  int k;
  int i, j;

  /* The GTP specification leaves the initial board configuration as
   * well as the board configuration after a boardsize command to the
   * discretion of the engine. We choose to start with up to 20 random
   * stones on the board.
   */
  clear_board();
  for (k = 0; k < 20; k++) {
    int color = rand() % 2 ? BLACK : WHITE;
    generate_move(&i, &j, color);
    play_move(i, j, color);
  }
}

/* Generate a move. */
void generate_move(int *i, int *j, int color)
{
  int moves[MAX_BOARD * MAX_BOARD];
  int num_moves = 0;
  int move;
  int ai, aj;
  int k;

  memset(moves, 0, sizeof(moves));
  for (ai = 0; ai < board_size; ai++)
    for (aj = 0; aj < board_size; aj++) {
      /* Consider moving at (ai, aj) if it is legal and not suicide. */
      if (legal_move(ai, aj, color)
      && !suicide(ai, aj, color)) {
    /* Further require the move not to be suicide for the opponent... */
    if (!suicide(ai, aj, OTHER_COLOR(color)))
      moves[num_moves++] = POS(ai, aj);
    else {
      /* ...however, if the move captures at least one stone,
           * consider it anyway.
       */
      for (k = 0; k < 4; k++) {
        int bi = ai + deltai[k];
        int bj = aj + deltaj[k];
        if (on_board(bi, bj) && get_board(bi, bj) == OTHER_COLOR(color)) {
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
  }
  else {
    /* But pass if no move was considered. */
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
