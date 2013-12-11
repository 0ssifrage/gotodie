#include <stdio.h>
#include <string.h>

#include "board.h"
#include "gotodie.h"
#include "gtp.h"

static int gtp_protocol_version(char *s);
static int gtp_name(char *s);
static int gtp_version(char *s);
static int gtp_known_command(char *s);
static int gtp_list_commands(char *s);
static int gtp_quit(char *s);
static int gtp_boardsize(char *s);
static int gtp_clear_board(char *s);
static int gtp_komi(char *s);
static int gtp_fixed_handicap(char *s);
static int gtp_place_free_handicap(char *s);
static int gtp_set_free_handicap(char *s);
static int gtp_play(char *s);
static int gtp_genmove(char *s);
static int gtp_final_score(char *s);
static int gtp_final_status_list(char *s);
static int gtp_showboard(char *s);

static struct gtp_command commands[] = {
    {"protocol_version", gtp_protocol_version},
    {"name", gtp_name},
    {"version", gtp_version},
    {"known_command", gtp_known_command},
    {"list_commands", gtp_list_commands},
    {"quit", gtp_quit},
    {"boardsize", gtp_boardsize},
    {"clear_board", gtp_clear_board},
    {"komi", gtp_komi},
    {"fixed_handicap", gtp_fixed_handicap},
    {"place_free_handicap", gtp_place_free_handicap},
    {"set_free_handicap", gtp_set_free_handicap},
    {"play", gtp_play},
    {"genmove", gtp_genmove},
    {"final_score", gtp_final_score},
    {"final_status_list", gtp_final_status_list},
    {"showboard", gtp_showboard},
    {NULL, NULL}
};

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    gtp_internal_set_boardsize(board_size);

    init_gotodie();

    gtp_main_loop(commands, stdin, stdout, NULL);

    return 0;
}

static int gtp_protocol_version(char *s)
{
    return gtp_success("2");
}

static int gtp_name(char *s)
{
    return gtp_success(GOTODIE_NAME);
}

static int gtp_version(char *s)
{
    return gtp_success(GOTODIE_VERSION);
}

static int gtp_known_command(char *s)
{
    int i;
    char command_name[GTP_BUFSIZE];

    if (sscanf(s, "%s", command_name) != 1)
        return gtp_success("false");

    for (i = 0; commands[i].name != NULL; i++)
        if (strcmp(command_name, commands[i].name) == 0)
            return gtp_success("true");

    return gtp_success("false");
}

static int gtp_list_commands(char *s)
{
    int i;

    gtp_start_response(GTP_SUCCESS);

    for (i = 0; commands[i].name != NULL; i++)
        gtp_printf("%s\n", commands[i].name);
    gtp_printf("\n");

    return GTP_OK;
}

static int gtp_quit(char *s)
{
    gtp_success("");
    return GTP_QUIT;
}

static int gtp_boardsize(char *s)
{
    int boardsize;

    if (sscanf(s, "%d", &boardsize) < 1)
        return gtp_failure("boardsize not an integer");

    if (boardsize < MIN_BOARD || boardsize > MAX_BOARD)
            return gtp_failure("unacceptable size");

    board_size = boardsize;
    gtp_internal_set_boardsize(boardsize);
    init_gotodie();

    return gtp_success("");
}

static int gtp_clear_board(char *s)
{
    clear_board();
    return gtp_success("");
}

static int gtp_komi(char *s)
{
    if (sscanf(s, "%f", &komi) < 1)
        return gtp_failure("komi not a float");

    return gtp_success("");
}

static int place_handicap(char *s, int fixed)
{
    int handicap;
    int m, n;
    int first_stone = 1;

    if (!board_empty())
        return gtp_failure("board not empty");

    if (sscanf(s, "%d", &handicap) < 1)
        return gtp_failure("handicap not an integer");

    if (handicap < 2)
        return gtp_failure("invalid handicap");

    if (fixed) {
        if (!place_fixed_handicap(handicap))
            return gtp_failure("invalid handicap");
    } else
        place_free_handicap(handicap);

    gtp_start_response(GTP_SUCCESS);
    for (m = 0; m < board_size; m++)
        for (n = 0; n < board_size; n++)
            if (board[POS(m, n)] != EMPTY) {
                if (first_stone)
                    first_stone = 0;
                else
                    gtp_printf(" ");
                gtp_mprintf("%m", m, n);
            }
    return gtp_finish_response();
}

static int gtp_fixed_handicap(char *s)
{
    return place_handicap(s, 1);
}

static int gtp_place_free_handicap(char *s)
{
    return place_handicap(s, 0);
}

static int gtp_set_free_handicap(char *s)
{
    int i, j;
    int n;
    int handicap = 0;

    if (!board_empty())
        return gtp_failure("board not empty");

    while ((n = gtp_decode_coord(s, &i, &j)) > 0) {
        s += n;
        if (board[POS(i, j)] != EMPTY) {
            clear_board();
            return gtp_failure("repeated vertex");
        }
        add_stone(POS(i, j), BLACK);
        handicap++;
    }

    if (sscanf(s, "%*s") != EOF) {
        clear_board();
        return gtp_failure("invalid coordinate");
    }

    if (handicap < 2 || handicap >= board_size * board_size) {
        clear_board();
        return gtp_failure("invalid handicap");
    }

    return gtp_success("");
}

static int gtp_play(char *s)
{
    int i, j;
    int color;

    if (!gtp_decode_move(s, &color, &i, &j))
        return gtp_failure("invalid color or coordinate");

    if (!is_legal(POS(i, j), color))
        return gtp_failure("illegal move");

    play_move(POS(i, j), color);
        return gtp_success("");
}

static int gtp_genmove(char *s)
{
    int i, j;
    int color = EMPTY;

    if (!gtp_decode_color(s, &color))
        return gtp_failure("invalid color");

    generate_move(&i, &j, color);
    play_move(POS(i, j), color);

    gtp_start_response(GTP_SUCCESS);
    gtp_mprintf("%m", i, j);
    return gtp_finish_response();
}

static int gtp_final_score(char *s)
{
    float score;
    score = compute_final_score(komi);

    if (score > 0.0)
        return gtp_success("W+%3.1f", score);
    if (score < 0.0)
        return gtp_success("B+%3.1f", -score);
    return gtp_success("0");
}

static int gtp_final_status_list(char *s)
{
    int n;
    int i, j;
    int status = UNKNOWN;
    char status_string[GTP_BUFSIZE];
    int first_string;

    if (sscanf(s, "%s %n", status_string, &n) != 1)
        return gtp_failure("missing status");

    if (strcmp(status_string, "alive") == 0)
        status = ALIVE;
    else if (strcmp(status_string, "dead") == 0)
        status = DEAD;
    else if (strcmp(status_string, "seki") == 0)
        status = SEKI;
    else
        return gtp_failure("invalid status");

    compute_final_status();

    gtp_start_response(GTP_SUCCESS);

    first_string = 1;
    for (i = 0; i < board_size; i++)
        for (j = 0; j < board_size; j++)
            if (get_final_status(i, j) == status) {
                int k;
                int stonei[MAX_BOARD * MAX_BOARD];
                int stonej[MAX_BOARD * MAX_BOARD];
                int num_stones = get_string(i, j, stonei, stonej);
                /* Clear the status so we don't find the string again. */
                for (k = 0; k < num_stones; k++)
                    set_final_status(stonei[k], stonej[k], UNKNOWN);

                if (first_string)
                    first_string = 0;
                else
                    gtp_printf("\n");

                gtp_print_vertices(num_stones, stonei, stonej);
            }

  return gtp_finish_response();
}

static int gtp_showboard(char *s)
{
    gtp_start_response(GTP_SUCCESS);
    gtp_printf("\n");
    showboard();
    return gtp_finish_response();
}
