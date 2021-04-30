#include <stdlib.h>
#include <signal.h>
#include "tty.h"
#include "loop.h"
#include "game.h"

void signal_int(int s) {
    (void)s;

    tty_normal();
    exit(0);
}

void init(void) {
    signal(SIGINT, signal_int);
    tty_raw();
    game_init();
}

int main() {
    init();
    loop();
    raise(SIGINT);
}
