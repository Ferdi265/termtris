#include <stdlib.h>
#include <signal.h>
#include "tetromino.h"
#include "tty.h"
#include "draw.h"
#include "game.h"
#include "util.h"

int game_score;
int game_lines;
int game_level;

bool game_pressed_space;
bool game_pressed_left;
bool game_pressed_up;
bool game_pressed_right;
bool game_pressed_down;

tetromino_id_t next_piece_id;
tetromino_t * next_piece;
color_t next_color;

color_t game_buffer[TETRIS_LINES][TETRIS_COLUMNS];

static tetromino_id_t cur_piece_id;
static tetromino_t * cur_piece;
static color_t cur_color;
static int cur_line;
static int cur_col;

static int cur_gravity;
static int gravity_counter;

#define MAX_CLEARED 4
static int num_cleared;
static int cleared_lines[MAX_CLEARED];

static bool failed;

void game_next_piece(void) {
    if (next_piece == NULL) return;

    cur_piece_id = next_piece_id;
    cur_piece = next_piece;
    cur_color = next_color;

    next_piece = util_random_piece(&next_piece_id);
    next_color = util_random_color();

    cur_line = 0;
    cur_col = util_center_piece(cur_piece);

    if (!util_piece_fits(cur_piece, cur_line, cur_col)) {
        cur_piece = NULL;
        failed = true;
    } else {
        util_place_piece(cur_piece, cur_line, cur_col, cur_color);
    }
}

void game_rotate_piece(void) {
    if (cur_piece == NULL) return;
    if (!util_piece_can_rotate(cur_piece, cur_line, cur_col, cur_piece_id)) return;

    util_rotate_piece(cur_piece, cur_line, cur_col, cur_color, cur_piece_id);
}

void game_move_piece(int dx, int dy) {
    if (cur_piece == NULL) return;
    if (!util_piece_can_move(cur_piece, cur_line, cur_col, dx, dy)) return;

    util_move_piece(cur_piece, cur_line, cur_col, cur_color, dx, dy);
    cur_line += dy;
    cur_col += dx;
}

void game_init(void) {
    next_piece = util_random_piece(&next_piece_id);
    next_color = util_random_color();
    game_next_piece();

    game_score = 0;
    game_lines = 0;
    game_level = 1;

    cur_gravity = 30;
    gravity_counter = cur_gravity;

    draw_init();
    draw_update();
}

void game_check_clear(void) {
    num_cleared = util_count_cleared(cleared_lines, MAX_CLEARED);
}

void game_fail_animation_update(void) {
    // TODO: fail animation
    raise(SIGINT);
}

void game_clear_animation_update(void) {
    // TODO: clear animation

    // shift in reverse order to not invalidate indices
    for (int i = num_cleared - 1; i >= 0; i--) {
        util_shift_lines(cleared_lines[i]);
    }

    num_cleared = 0;
}

void game_controls_update(void) {
    if (game_pressed_space) {
        game_rotate_piece();
    }

    if (game_pressed_left) {
        game_move_piece(-1, 0);
    }

    if (game_pressed_right) {
        game_move_piece(1, 0);
    }

    // TODO: fast fall
}

void game_gravity_update(void) {
    if (gravity_counter > 0) {
        gravity_counter--;
        return;
    }

    gravity_counter = cur_gravity;

    if (cur_piece == NULL) {
        game_next_piece();
    } else if (util_piece_can_move(cur_piece, cur_line, cur_col, 0, 1)) {
        game_move_piece(0, 1);
    } else {
        cur_piece = NULL;
        game_check_clear();
    }
}

void game_tick(void) {
    if (failed) {
        game_fail_animation_update();
    } else if (num_cleared > 0) {
        game_clear_animation_update();
    } else {
        game_controls_update();
        game_gravity_update();
    }

    draw_update();
    game_pressed_space = game_pressed_left = game_pressed_up = game_pressed_down = game_pressed_right = false;
}

void game_key(key_id_t key) {
    switch (key) {
        case KEY_UP:
            game_pressed_up = true;
            break;
        case KEY_DOWN:
            game_pressed_down = true;
            break;
        case KEY_RIGHT:
            game_pressed_right = true;
            break;
        case KEY_LEFT:
            game_pressed_left = true;
            break;
        case KEY_SPACE:
            game_pressed_space = true;
            break;
        case KEY_ESC:
            raise(SIGINT);
            break;
        default:
            break;
    }
}

