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

static int level_start_lines;

#define MAX_CLEARED 4
static int num_cleared;
static int cleared_lines[MAX_CLEARED];
static int clear_animation_counter;

static int score_table[MAX_CLEARED + 1] = {
    0,
    40,
    100,
    300,
    1200
};

#define MAX_GRAVITY_LEVEL 29
static int gravity_table[MAX_GRAVITY_LEVEL + 1] = {
    48,
    43,
    38,
    33,
    28,
    23,
    18,
    13,
    8,
    6,
    5,
    5,
    5,
    4,
    4,
    4,
    3,
    3,
    3,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    1
};

static int cur_gravity;
static int gravity_counter;

static bool failed;

void game_next_piece(void) {
    if (next_piece == NULL) return;

    cur_piece_id = next_piece_id;
    cur_piece = next_piece;
    cur_color = next_color;

    next_piece = util_random_piece(&next_piece_id, cur_piece_id.shape);
    next_color = util_random_color();

    cur_line = util_top_align_piece(cur_piece);
    cur_col = util_center_align_piece(cur_piece);

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

    util_rotate_piece(&cur_piece, cur_line, cur_col, cur_color, &cur_piece_id);
}

void game_move_piece(int dx, int dy) {
    if (cur_piece == NULL) return;
    if (!util_piece_can_move(cur_piece, cur_line, cur_col, dx, dy)) return;

    util_move_piece(cur_piece, &cur_line, &cur_col, cur_color, dx, dy);
}

void game_move_gravity(void) {
    if (cur_piece == NULL) {
        game_next_piece();
    } else if (util_piece_can_move(cur_piece, cur_line, cur_col, 0, 1)) {
        game_move_piece(0, 1);
    } else {
        cur_piece = NULL;
    }
}

void game_init(int start_level) {
    next_piece = util_random_piece(&next_piece_id, NUM_TETROMINOS);
    next_color = util_random_color();
    game_next_piece();

    game_score = 0;
    game_lines = 0;
    game_level = start_level;
    level_start_lines = 0;

    if (game_level < MAX_GRAVITY_LEVEL) {
        cur_gravity = gravity_table[game_level];
    } else {
        cur_gravity = gravity_table[MAX_GRAVITY_LEVEL];
    }
    gravity_counter = cur_gravity;

    draw_init();
    draw_update();
}

#define CLAMP(var, max) var = var > max ? max : var;
void game_score_update(void) {
    game_lines += num_cleared;
    game_score += score_table[num_cleared] * (game_level + 1);

    if (game_lines - level_start_lines > 10) {
        game_level++;
        level_start_lines = game_lines;
        if (game_level < MAX_GRAVITY_LEVEL) {
            cur_gravity = gravity_table[game_level];
        } else {
            cur_gravity = gravity_table[MAX_GRAVITY_LEVEL];
        }
    }

    CLAMP(game_score, 999999);
    CLAMP(game_lines, 999);
    CLAMP(game_level, 99);
}

void game_fail_animation_update(void) {
    raise(SIGINT);
}

#define LINE_CLEAR_STEPS ((TETRIS_COLUMNS + 1) / 2)
void game_check_clear(void) {
    num_cleared = util_count_cleared(cleared_lines, MAX_CLEARED);
    clear_animation_counter = 2 * LINE_CLEAR_STEPS;
}

void game_clear_animation_overwrite(int offset, color_t color) {
    for (int i = 0; i < num_cleared; i++) {
        game_buffer[cleared_lines[i]][offset] = color;
        game_buffer[cleared_lines[i]][TETRIS_COLUMNS - offset - 1] = color;
    }
}

void game_clear_animation_update(void) {
    if (clear_animation_counter == 0) {
        // shift in reverse order to not invalidate indices
        for (int i = num_cleared - 1; i >= 0; i--) {
            util_shift_lines(cleared_lines[i]);
        }

        game_score_update();
        num_cleared = 0;
    } else {
        clear_animation_counter--;

        if (num_cleared == MAX_CLEARED) {
            if (clear_animation_counter >= LINE_CLEAR_STEPS) {
                game_clear_animation_overwrite(clear_animation_counter - LINE_CLEAR_STEPS, C_WHITE);
            } else {
                game_clear_animation_overwrite(clear_animation_counter, C_NONE);
            }
        } else {
            game_clear_animation_overwrite(clear_animation_counter / 2, C_NONE);
        }
    }
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

    if (game_pressed_down) {
        game_move_gravity();
    }
}

void game_gravity_update(void) {
    if (gravity_counter > 0) {
        gravity_counter--;
        return;
    }

    gravity_counter = cur_gravity;
    game_move_gravity();
}

void game_tick(void) {
    if (failed) {
        game_fail_animation_update();
    } else if (num_cleared > 0) {
        game_clear_animation_update();
    } else {
        game_controls_update();
        game_gravity_update();

        if (cur_piece == NULL) {
            game_check_clear();
        }
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

