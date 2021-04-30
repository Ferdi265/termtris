#ifndef TERMTRIS_UTIL_H_
#define TERMTRIS_UTIL_H_

#include <stdbool.h>
#include "tetromino.h"
#include "game.h"

tetromino_t * util_random_piece(tetromino_id_t * id);
color_t util_random_color(void);

bool util_piece_fits(tetromino_t * piece, int line, int col);
void util_place_piece(tetromino_t * piece, int line, int col, color_t color);
void util_erase_piece(tetromino_t * piece, int line, int col);

#endif
