#ifndef TERMTRIS_TETROMINO_H_
#define TERMTRIS_TETROMINO_H_

#include "game.h"

typedef struct {
    bool blocks[NEXTBOX_LINES][NEXTBOX_COLUMNS];
    int size;
} tetromino_t;

#define NUM_TETROMINOS 7
#define NUM_ROTATIONS 4
extern tetromino_t tetromino_shapes[NUM_TETROMINOS][NUM_ROTATIONS];

#endif
