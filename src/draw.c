#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "draw.h"
#include "game.h"

#define LEN_BOXCHAR 3
#define LEN_CLEAR 10
#define LEN_RESET 4
#define LEN_BLOCK 7
#define LEN_LINE_END 2
#define INFOBOX_LINES 4
#define INFOBOX_COLUMNS 17
#define NEXTBOX_LEFT_OFFSET 5
#define NEXTBOX_RIGHT_OFFSET 4
#define LEN_SAVE 3
#define LEN_RESTORE 3
#define LEN_MOVE_TOP 4
#define LEN_INFOBOX_LINE_END 9
#define LEN_INPUT_CHAR 3
#define LEN_INPUT_LINE (6 * 2 + 5 * LEN_INPUT_CHAR)
char draw_buffer[
    LEN_CLEAR +
    (1 + 2 * TETRIS_COLUMNS + 1) * LEN_BOXCHAR + LEN_LINE_END +
    (LEN_BOXCHAR + LEN_BLOCK * TETRIS_COLUMNS + LEN_RESET + LEN_BOXCHAR + LEN_LINE_END) * TETRIS_LINES +
    (1 + 2 * TETRIS_COLUMNS + 1) * LEN_BOXCHAR +
    LEN_SAVE +
    LEN_MOVE_TOP + (1 + INFOBOX_COLUMNS + 1) * LEN_BOXCHAR + LEN_INFOBOX_LINE_END +
    (LEN_BOXCHAR + INFOBOX_COLUMNS + LEN_BOXCHAR + LEN_INFOBOX_LINE_END) * INFOBOX_LINES +
    (1 + INFOBOX_COLUMNS + 1) * LEN_BOXCHAR + LEN_INFOBOX_LINE_END +
    (1 + INFOBOX_COLUMNS + 1) * LEN_BOXCHAR + LEN_INFOBOX_LINE_END +
    LEN_BOXCHAR + INFOBOX_COLUMNS + LEN_BOXCHAR + LEN_INFOBOX_LINE_END +
    (LEN_BOXCHAR + NEXTBOX_LEFT_OFFSET + LEN_BLOCK * NEXTBOX_COLUMNS + LEN_RESET + NEXTBOX_RIGHT_OFFSET + LEN_BOXCHAR + LEN_INFOBOX_LINE_END) * NEXTBOX_LINES +
    (1 + INFOBOX_COLUMNS + 1) * LEN_BOXCHAR + LEN_INFOBOX_LINE_END +
    (1 + INFOBOX_COLUMNS + 1) * LEN_BOXCHAR + LEN_INFOBOX_LINE_END +
    LEN_BOXCHAR + INFOBOX_COLUMNS + LEN_BOXCHAR + LEN_INFOBOX_LINE_END +
    LEN_BOXCHAR + LEN_INPUT_LINE + LEN_BOXCHAR + LEN_INFOBOX_LINE_END +
    (1 + INFOBOX_COLUMNS + 1) * LEN_BOXCHAR +
    LEN_RESTORE + LEN_LINE_END
];

#define APPEND(p, s, l) do { memcpy(p, s, l); p += l; } while (0)
#define REPEAT(n, ...) for (int i = 0; i < n; i++) __VA_ARGS__
char * draw_game_offset;
char * draw_next_offset;
char * draw_score_offset;
char * draw_lines_offset;
char * draw_level_offset;
char * draw_space_offset;
char * draw_left_offset;
char * draw_up_offset;
char * draw_down_offset;
char * draw_right_offset;
void draw_init(void) {
    char * ptr = draw_buffer;
    APPEND(ptr, "\x1b[2J\x1b[0;0H", LEN_CLEAR);
    APPEND(ptr, "┌", LEN_BOXCHAR);
    REPEAT(2 * TETRIS_COLUMNS, APPEND(ptr, "─", LEN_BOXCHAR));
    APPEND(ptr, "┐", LEN_BOXCHAR);
    APPEND(ptr, "\r\n", LEN_LINE_END);
    draw_game_offset = ptr;
    REPEAT(TETRIS_LINES, {
        APPEND(ptr, "│", LEN_BOXCHAR);
        REPEAT(TETRIS_COLUMNS, APPEND(ptr, "\x1b[00m  ", LEN_BLOCK));
        APPEND(ptr, "\x1b[0m", LEN_RESET);
        APPEND(ptr, "│", LEN_BOXCHAR);
        APPEND(ptr, "\r\n", LEN_LINE_END);
    });
    APPEND(ptr, "└", LEN_BOXCHAR);
    REPEAT(2 * TETRIS_COLUMNS, APPEND(ptr, "─", LEN_BOXCHAR));
    APPEND(ptr, "┘", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[s", LEN_SAVE);
    APPEND(ptr, "\x1b[1d", LEN_MOVE_TOP);
    APPEND(ptr, "┌", LEN_BOXCHAR);
    REPEAT(INFOBOX_COLUMNS, APPEND(ptr, "─", LEN_BOXCHAR));
    APPEND(ptr, "┐", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, " T E R M T R I S ", INFOBOX_COLUMNS);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "                 ", INFOBOX_COLUMNS);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, " SCORE  LINES LV ", INFOBOX_COLUMNS);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "│", LEN_BOXCHAR);
    draw_score_offset = ptr + 1;
    draw_lines_offset = ptr + 1 + 6 + 3;
    draw_level_offset = ptr + 1 + 6 + 3 + 3 + 1;
    APPEND(ptr, " 000000   000 00 ", INFOBOX_COLUMNS);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "└", LEN_BOXCHAR);
    REPEAT(INFOBOX_COLUMNS, APPEND(ptr, "─", LEN_BOXCHAR));
    APPEND(ptr, "┘", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "┌", LEN_BOXCHAR);
    REPEAT(INFOBOX_COLUMNS, APPEND(ptr, "─", LEN_BOXCHAR));
    APPEND(ptr, "┐", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "     N E X T     ", INFOBOX_COLUMNS);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    draw_next_offset = ptr;
    REPEAT(NEXTBOX_LINES, {
        APPEND(ptr, "│", LEN_BOXCHAR);
        REPEAT(NEXTBOX_LEFT_OFFSET, APPEND(ptr, " ", 1));
        REPEAT(NEXTBOX_COLUMNS, APPEND(ptr, "\x1b[00m  ", LEN_BLOCK));
        APPEND(ptr, "\x1b[0m", LEN_RESET);
        REPEAT(NEXTBOX_RIGHT_OFFSET, APPEND(ptr, " ", 1));
        APPEND(ptr, "│", LEN_BOXCHAR);
        APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    });
    APPEND(ptr, "└", LEN_BOXCHAR);
    REPEAT(INFOBOX_COLUMNS, APPEND(ptr, "─", LEN_BOXCHAR));
    APPEND(ptr, "┘", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "┌", LEN_BOXCHAR);
    REPEAT(INFOBOX_COLUMNS, APPEND(ptr, "─", LEN_BOXCHAR));
    APPEND(ptr, "┐", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "    I N P U T    ", INFOBOX_COLUMNS);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "│", LEN_BOXCHAR);
    draw_space_offset = ptr + 2;
    draw_left_offset = ptr + 2 + 5;
    draw_up_offset = ptr + 2 + 5 + 5;
    draw_down_offset = ptr + 2 + 5 + 5 + 5;
    draw_right_offset = ptr + 2 + 5 + 5 + 5 + 5;
    APPEND(ptr, "  •  ←  ↑  →  ↓  ", LEN_INPUT_LINE);
    APPEND(ptr, "│", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[1B\x1b[19D", LEN_INFOBOX_LINE_END);
    APPEND(ptr, "└", LEN_BOXCHAR);
    REPEAT(INFOBOX_COLUMNS, APPEND(ptr, "─", LEN_BOXCHAR));
    APPEND(ptr, "┘", LEN_BOXCHAR);
    APPEND(ptr, "\x1b[u", LEN_RESTORE);
    APPEND(ptr, "\r\n", LEN_LINE_END);
}

#define LEN_LINE (LEN_BOXCHAR + LEN_BLOCK * TETRIS_COLUMNS + LEN_RESET + LEN_BOXCHAR + LEN_LINE_END)
#define LINE_OFFSET (LEN_BOXCHAR)
#define LEN_NB_LINE (LEN_BOXCHAR + NEXTBOX_LEFT_OFFSET + LEN_BLOCK * NEXTBOX_COLUMNS + LEN_RESET + NEXTBOX_RIGHT_OFFSET + LEN_BOXCHAR + LEN_INFOBOX_LINE_END)
#define NB_LINE_OFFSET (LEN_BOXCHAR + NEXTBOX_LEFT_OFFSET)
#define BLOCK_OFFSET 2
void draw_update(void) {
    for (int line = 0; line < TETRIS_LINES; line++) {
        for (int col = 0; col < TETRIS_COLUMNS; col++) {
            char * ptr = draw_game_offset + line * LEN_LINE + LINE_OFFSET + col * LEN_BLOCK + BLOCK_OFFSET;
            color_t color = game_buffer[line][col];
            if (color == C_NONE) {
                ptr[0] = '0';
                ptr[1] = '0';
            } else {
                ptr[0] = '4';
                ptr[1] = color;
            }
        }
    }

    color_t color = next_color;
    int start_line = 0, start_col = 0;
    if (next_piece->size == 2) {
        start_line = 1;
        start_col = 1;
    }
    for (int line = 0; line < NEXTBOX_LINES; line++) {
        for (int col = 0; col < NEXTBOX_COLUMNS; col++) {
            char * ptr = draw_next_offset + line * LEN_NB_LINE + NB_LINE_OFFSET + col * LEN_BLOCK + BLOCK_OFFSET;

            bool has_block;
            if (line < start_line || col < start_col) {
                has_block = false;
            } else {
                has_block = next_piece->blocks[line - start_line][col - start_col];
            }

            if (!has_block) {
                ptr[0] = '0';
                ptr[1] = '0';
            } else {
                ptr[0] = '4';
                ptr[1] = color;
            }
        }
    }

    if (game_pressed_space) {
        memcpy(draw_space_offset, "•", LEN_INPUT_CHAR);
    } else {
        memcpy(draw_space_offset, " \x01\x01", LEN_INPUT_CHAR);
    }

    if (game_pressed_left) {
        memcpy(draw_left_offset, "←", LEN_INPUT_CHAR);
    } else {
        memcpy(draw_left_offset, " \x01\x01", LEN_INPUT_CHAR);
    }

    if (game_pressed_up) {
        memcpy(draw_up_offset, "↑", LEN_INPUT_CHAR);
    } else {
        memcpy(draw_up_offset, " \x01\x01", LEN_INPUT_CHAR);
    }

    if (game_pressed_down) {
        memcpy(draw_down_offset, "↓", LEN_INPUT_CHAR);
    } else {
        memcpy(draw_down_offset, " \x01\x01", LEN_INPUT_CHAR);
    }

    if (game_pressed_right) {
        memcpy(draw_right_offset, "→", LEN_INPUT_CHAR);
    } else {
        memcpy(draw_right_offset, " \x01\x01", LEN_INPUT_CHAR);
    }

    char sprintf_buf[8];
    snprintf(sprintf_buf, sizeof sprintf_buf, "%06d", game_score);
    memcpy(draw_score_offset, sprintf_buf, 6);
    snprintf(sprintf_buf, sizeof sprintf_buf, "%03d", game_lines);
    memcpy(draw_lines_offset, sprintf_buf, 3);
    snprintf(sprintf_buf, sizeof sprintf_buf, "%02d", game_level);
    memcpy(draw_level_offset, sprintf_buf, 2);

    write(STDOUT_FILENO, draw_buffer, sizeof draw_buffer);
}
