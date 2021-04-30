#ifndef TERMTRIS_TETROMINO_H_
#define TERMTRIS_TETROMINO_H_

#include <stdbool.h>

#define TETROMINO_LINES 4
#define TETROMINO_COLUMNS 4

typedef struct {
    bool blocks[TETROMINO_LINES][TETROMINO_COLUMNS];
    int size;
} tetromino_t;

enum {
    TETROMINO_I,
    TETROMINO_O,
    TETROMINO_T,
    TETROMINO_L,
    TETROMINO_J,
    TETROMINO_S,
    TETROMINO_Z,
};

#define NUM_TETROMINOS 7
#define NUM_ROTATIONS 4
extern tetromino_t tetromino_shapes[NUM_TETROMINOS][NUM_ROTATIONS];

#endif
