#ifndef TERMTRIS_UTIL_H_
#define TERMTRIS_UTIL_H_

#include <stdbool.h>
#include "tetromino.h"
#include "game.h"

tetromino_t * util_random_piece(tetromino_id_t * id);
color_t util_random_color(void);

int util_center_piece(tetromino_t * piece);
bool util_piece_fits(tetromino_t * piece, int line, int col);
bool util_piece_can_move(tetromino_t * piece, int line, int col, int dx, int dy);
bool util_piece_can_rotate(tetromino_t * piece, int line, int col, tetromino_id_t id);
void util_erase_piece(tetromino_t * piece, int line, int col);
void util_place_piece(tetromino_t * piece, int line, int col, color_t color);
void util_move_piece(tetromino_t * piece, int * line, int * col, color_t color, int dx, int dy);
void util_rotate_piece(tetromino_t ** piece, int line, int col, color_t color, tetromino_id_t * id);

int util_count_cleared(int * cleared_lines, int max_cleared);
void util_shift_lines(int cleared_line);

#endif
