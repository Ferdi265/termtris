#ifndef TERMTRIS_TTY_H_
#define TERMTRIS_TTY_H_

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

void tty_raw(void);
void tty_normal(void);
int tty_key(void);

#endif
