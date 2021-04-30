#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "game.h"
#include "tetromino.h"

tetromino_t * util_random_piece(tetromino_id_t * id, unsigned char last_shape) {
    unsigned int shape = rand() % (NUM_TETROMINOS + 1);
    unsigned int rotation = rand() % NUM_ROTATIONS;

    if (shape == last_shape || shape == NUM_TETROMINOS) {
        shape = rand() % NUM_TETROMINOS;
    }

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

int util_center_piece(tetromino_t * piece) {
    if (piece->size == 2) {
        return TETRIS_COLUMNS / 2 - 1;
    } else {
        return TETRIS_COLUMNS / 2 - 2;
    }
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

bool util_piece_can_move(tetromino_t * piece, int line, int col, int dx, int dy) {
    for (int cur_line = 0; cur_line < TETROMINO_LINES; cur_line++) {
        for (int cur_col = 0; cur_col < TETROMINO_COLUMNS; cur_col++) {
            if (
                piece->blocks[cur_line][cur_col] && (
                    (cur_line + dy < 0 || cur_line + dy >= TETROMINO_LINES) ||
                    (cur_col + dx < 0 || cur_col + dx >= TETROMINO_COLUMNS) ||
                    !piece->blocks[cur_line + dy][cur_col + dx]
                )
            ) {
                if (
                    line + cur_line + dy < 0 || line + cur_line + dy >= TETRIS_LINES ||
                    col + cur_col + dx < 0 || col + cur_col + dx >= TETRIS_COLUMNS ||
                    game_buffer[line + cur_line + dy][col + cur_col + dx] != C_NONE
                ) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool util_piece_can_rotate(tetromino_t * piece, int line, int col, tetromino_id_t id) {
    tetromino_t * rotated_piece = &tetromino_shapes[id.shape][(id.rotation + 1) % NUM_ROTATIONS];
    for (int cur_line = 0; cur_line < TETROMINO_LINES; cur_line++) {
        for (int cur_col = 0; cur_col < TETROMINO_COLUMNS; cur_col++) {
            if (!piece->blocks[cur_line][cur_col] && rotated_piece->blocks[cur_line][cur_col]) {
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

void util_erase_piece(tetromino_t * piece, int line, int col) {
    util_place_piece(piece, line, col, C_NONE);
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

void util_move_piece(tetromino_t * piece, int * line, int * col, color_t color, int dx, int dy) {
    util_erase_piece(piece, *line, *col);
    util_place_piece(piece, *line + dy, *col + dx, color);
    *line += dy;
    *col += dx;
}

void util_rotate_piece(tetromino_t ** piece, int line, int col, color_t color, tetromino_id_t * id) {
    tetromino_id_t rotated_id = { .shape = id->shape, .rotation = (id->rotation + 1) % NUM_ROTATIONS };
    tetromino_t * rotated_piece = &tetromino_shapes[rotated_id.shape][rotated_id.rotation];
    util_erase_piece(*piece, line, col);
    util_place_piece(rotated_piece, line, col, color);
    *id = rotated_id;
    *piece = rotated_piece;
}

int util_count_cleared(int * cleared_lines, int max_cleared) {
    int num_cleared = 0;

    for (int line = TETRIS_LINES - 1; line >= 0; line--) {
        bool cleared = true;
        for (int col = 0; col < TETRIS_COLUMNS; col++) {
            if (game_buffer[line][col] == C_NONE) {
                cleared = false;
                break;
            }
        }

        if (cleared) {
            *cleared_lines = line;
            cleared_lines++;
            num_cleared++;

            if (num_cleared >= max_cleared) {
                break;
            }
        }
    }

    return num_cleared;
}

void util_shift_lines(int cleared_line) {
    int num_shifted = cleared_line;
    memmove(game_buffer[1], game_buffer[0], num_shifted * TETRIS_COLUMNS);
    memset(game_buffer[0], C_NONE, TETRIS_COLUMNS);
}
