#include <stdlib.h>
#include "util.h"
#include "game.h"
#include "tetromino.h"

tetromino_t * util_random_piece(tetromino_id_t * id) {
    int shape = rand() % NUM_TETROMINOS;
    int rotation = rand() % NUM_ROTATIONS;

    if (id != NULL) {
        id->shape = shape;
        id->rotation = rotation;
    }

    return &tetromino_shapes[shape][rotation];
}

color_t util_random_color(void) {
    // generate one of red, green, yellow, blue, magenta
    int color = rand() % 5;
    return color + '1';
}

bool util_piece_fits(tetromino_t * piece, int line, int col) {
    for (int cur_line = 0; cur_line < TETROMINO_LINES; cur_line++) {
        for (int cur_col = 0; cur_col < TETROMINO_COLUMNS; cur_col++) {
            if (piece->blocks[cur_line][cur_col]) {
                if (
                    line + cur_line < 0 || line + cur_line >= TETRIS_LINES ||
                    col + cur_col < 0 || col + cur_col >= TETRIS_COLUMNS ||
                    game_buffer[line + cur_line][col + cur_col] != C_NONE
                ) {
                    return false;
                }
            }
        }
    }

    return true;
}

void util_place_piece(tetromino_t * piece, int line, int col, color_t color) {
    for (int cur_line = 0; cur_line < TETROMINO_LINES; cur_line++) {
        for (int cur_col = 0; cur_col < TETROMINO_COLUMNS; cur_col++) {
            if (piece->blocks[cur_line][cur_col]) {
                game_buffer[line + cur_line][col + cur_col] = color;
            }
        }
    }
}

void util_erase_piece(tetromino_t * piece, int line, int col) {
    util_place_piece(piece, line, col, C_NONE);
}
