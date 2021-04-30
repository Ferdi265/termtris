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

tetromino_id_t cur_piece_id;
tetromino_t * cur_piece;
char cur_color;
int cur_line;
int cur_col;

tetromino_id_t next_piece_id;
tetromino_t * next_piece;
color_t next_color;

color_t game_buffer[TETRIS_LINES][TETRIS_COLUMNS];

void game_next_piece(void) {
    cur_piece_id = next_piece_id;
    cur_piece = next_piece;
    cur_color = next_color;

    next_piece = util_random_piece(&next_piece_id);
    next_color = util_random_color();

    cur_line = 0;
    if (cur_piece->size == 2) {
        cur_col = 4;
    } else {
        cur_col = 3;
    }

    
    // TODO: place piece
}

void game_init(void) {
    game_buffer[5][4] = C_MAGENTA;
    game_buffer[6][3] = C_MAGENTA;
    game_buffer[6][4] = C_MAGENTA;
    game_buffer[6][5] = C_MAGENTA;

    game_buffer[2][2] = C_BLUE;
    game_buffer[2][3] = C_BLUE;
    game_buffer[3][2] = C_BLUE;
    game_buffer[4][2] = C_BLUE;

    next_piece = &tetromino_shapes[TETROMINO_S][0];
    next_color = C_GREEN;

    game_score = 777777;
    game_lines = 42;
    game_level = 17;

    draw_init();
    draw_update();
}

void game_tick(void) {
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

