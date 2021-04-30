#include <unistd.h>
#include <sys/select.h>
#include "tty.h"
#include "time.h"
#include "loop.h"
#include "game.h"

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
            key_id_t key = tty_key();
            if (key != KEY_NONE) {
                game_key(key);
            }
        }
    }
}
