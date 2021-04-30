#include <unistd.h>
#include <termios.h>
#include "tty.h"

static struct termios tty_state_old;

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

key_id_t tty_key(void) {
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
