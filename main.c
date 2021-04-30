#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/timerfd.h>

enum {
    KEY_UNKNOWN = -1,
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_SPACE,
    KEY_ESC,
};

struct termios tty_state_old;

void tty_raw(void) {
    struct termios state;

    tcgetattr(STDIN_FILENO, &state);
    tty_state_old = state;

    state.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
    state.c_oflag &= ~(OPOST);
    state.c_cflag |= (CS8);
    state.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    state.c_lflag &= ~ECHO;
    state.c_cc[VMIN] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &state);
}

void tty_normal(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty_state_old);
}

int tty_key(void) {
    char c;
    int num;

    num = read(STDIN_FILENO, &c, 1);
    if (num == 0) return KEY_NONE;

    switch (c) {
        case 'w':
            return KEY_UP;
        case 's':
            return KEY_DOWN;
        case 'd':
            return KEY_RIGHT;
        case 'a':
            return KEY_LEFT;
        case ' ':
            return KEY_SPACE;
        case '\x1b':
            break;
        default:
            return KEY_UNKNOWN;
    }

    num = read(STDIN_FILENO, &c, 1);
    if (num == 0) return KEY_ESC;

    switch (c) {
        case '[':
            break;
        default:
            return KEY_UNKNOWN;
    }

    num = read(STDIN_FILENO, &c, 1);
    if (num == 0) return KEY_UNKNOWN;

    switch (c) {
        case 'A':
            return KEY_UP;
        case 'B':
            return KEY_DOWN;
        case 'C':
            return KEY_RIGHT;
        case 'D':
            return KEY_LEFT;
        default:
            return KEY_UNKNOWN;
    }
}

void signal_int(int s) {
    (void)s;

    tty_normal();
    exit(0);
}

void game_init(void);
void init(void) {
    signal(SIGINT, signal_int);
    tty_raw();
    game_init();
}

long long time_usec(void) {
    struct timeval time;
    gettimeofday(&time, NULL);

    return time.tv_sec * 1000000LL + time.tv_usec;
}

long long time_rate = 16666;
long long time_now;
struct timeval time_timeout;
bool time_update_timeout(void) {
    long long now = time_usec();
    long long diff = now - time_now;
    time_now = now;

    if (diff > time_timeout.tv_usec) {
        time_timeout.tv_usec = time_rate;
        return true;
    } else {
        time_timeout.tv_usec -= diff;
        return false;
    }
}

void game_tick(void);
void game_key(int key);
void loop(void) {
    fd_set rfds;

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        int res = select(1, &rfds, NULL, NULL, &time_timeout);
        if (res == -1) {
            break;
        }

        if (time_update_timeout()) {
            game_tick();
        } else {
            int key = tty_key();
            if (key != KEY_NONE) {
                game_key(key);
            }
        }
    }
}

#define LEN_BOXCHAR 3
#define LEN_CLEAR 10
#define LEN_RESET 4
#define LEN_BLOCK 7
#define LEN_LINE_END 2
#define TETRIS_LINES 20
#define TETRIS_COLUMNS 10
#define INFOBOX_LINES 4
#define INFOBOX_COLUMNS 17
#define NEXTBOX_LINES 4
#define NEXTBOX_COLUMNS 4
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

enum {
    C_NONE = 0,
    C_BLACK = '0',
    C_RED = '1',
    C_GREEN = '2',
    C_YELLOW = '3',
    C_BLUE = '4',
    C_MAGENTA = '5',
    C_WHITE = '7'
};

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

#define LEN_LINE (LEN_BOXCHAR + LEN_BLOCK * TETRIS_COLUMNS + LEN_RESET + LEN_BOXCHAR + LEN_LINE_END)
#define LINE_OFFSET (LEN_BOXCHAR)
#define LEN_NB_LINE (LEN_BOXCHAR + NEXTBOX_LEFT_OFFSET + LEN_BLOCK * NEXTBOX_COLUMNS + LEN_RESET + NEXTBOX_RIGHT_OFFSET + LEN_BOXCHAR + LEN_INFOBOX_LINE_END)
#define NB_LINE_OFFSET (LEN_BOXCHAR + NEXTBOX_LEFT_OFFSET)
#define BLOCK_OFFSET 2
void draw_update(void) {
    for (int line = 0; line < TETRIS_LINES; line++) {
        for (int col = 0; col < TETRIS_COLUMNS; col++) {
            char * ptr = draw_game_offset + line * LEN_LINE + LINE_OFFSET + col * LEN_BLOCK + BLOCK_OFFSET;
            char color = game_buffer[line][col];
            if (color == 0) {
                ptr[0] = '0';
                ptr[1] = '0';
            } else {
                ptr[0] = '4';
                ptr[1] = color;
            }
        }
    }

    for (int line = 0; line < NEXTBOX_LINES; line++) {
        for (int col = 0; col < NEXTBOX_COLUMNS; col++) {
            char * ptr = draw_next_offset + line * LEN_NB_LINE + NB_LINE_OFFSET + col * LEN_BLOCK + BLOCK_OFFSET;
            char color = next_buffer[line][col];
            if (color == 0) {
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

int main() {
    init();
    loop();
    raise(SIGINT);
}
