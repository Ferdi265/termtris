#include <stdnoreturn.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "tty.h"
#include "loop.h"
#include "game.h"

void signal_int(int s) {
    (void)s;

    tty_normal();
    exit(0);
}

noreturn void usage(void) {
    printf("usage: termtris [start_level]\n");
    exit(1);
}

void init(void) {
    signal(SIGINT, signal_int);
    tty_raw();
    srand(time(NULL));
}

int main(int argc, char ** argv) {
    int start_level;
    if (argc == 1) {
        start_level = 0;
    } else if (argc == 2) {
        start_level = strtoul(argv[1], NULL, 10);
        if (start_level < 0 || start_level > 99) {
            usage();
        }
    } else {
        usage();
    }

    init();
    game_init(start_level);

    loop();
    raise(SIGINT);
}
