#ifndef TERMTRIS_GAME_H_
#define TERMTRIS_GAME_H_

#include <stdbool.h>
#include "tty.h"
#include "tetromino.h"

typedef enum {
    C_NONE = 0,
    C_BLACK = '0',
    C_RED = '1',
    C_GREEN = '2',
    C_YELLOW = '3',
    C_BLUE = '4',
    C_MAGENTA = '5',
    C_WHITE = '7'
} color_t;

#define TETRIS_LINES 20
#define TETRIS_COLUMNS 10
#define NEXTBOX_LINES 4
#define NEXTBOX_COLUMNS 4

extern int game_score;
extern int game_lines;
extern int game_level;
extern bool game_pressed_space;
extern bool game_pressed_left;
extern bool game_pressed_up;
extern bool game_pressed_right;
extern bool game_pressed_down;
extern tetromino_t * next_piece;
extern color_t next_color;
extern color_t game_buffer[TETRIS_LINES][TETRIS_COLUMNS];

void game_init(void);
void game_tick(void);
void game_key(key_id_t key);

#endif
