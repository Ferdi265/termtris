#include <signal.h>
#include "tty.h"
#include "draw.h"
#include "game.h"

unsigned int game_score;
unsigned int game_lines;
unsigned int game_level;
bool game_pressed_space;
bool game_pressed_left;
bool game_pressed_up;
bool game_pressed_right;
bool game_pressed_down;
unsigned char game_buffer[TETRIS_LINES][TETRIS_COLUMNS];
unsigned char next_buffer[NEXTBOX_LINES][NEXTBOX_COLUMNS];

void game_init(void) {
    game_buffer[5][4] = C_MAGENTA;
    game_buffer[6][3] = C_MAGENTA;
    game_buffer[6][4] = C_MAGENTA;
    game_buffer[6][5] = C_MAGENTA;

    game_buffer[2][2] = C_BLUE;
    game_buffer[2][3] = C_BLUE;
    game_buffer[3][2] = C_BLUE;
    game_buffer[4][2] = C_BLUE;

    next_buffer[1][1] = C_GREEN;
    next_buffer[1][2] = C_GREEN;
    next_buffer[2][0] = C_GREEN;
    next_buffer[2][1] = C_GREEN;

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

void game_key(int key) {
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
    }
}

