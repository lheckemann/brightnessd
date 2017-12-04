#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <poll.h>

typedef unsigned long int u;

static u get_env_u(const char* name, u default_) {
    const char* env = getenv(name);
    if (env == NULL) {
        return default_;
    }

    char *check;
    errno = 0;
    unsigned long int ret = strtoul(env, &check, 10);
    // if strtoul gave us an error condition
    // or the integer didn't go to the end of the string, consider it a failure
    if (errno || *check) {
        fprintf(
            stderr,
            "Could not interpret env var %s (value '%s') as integer, "
            "using default value of %lu\n",
            name,
            env,
            default_
        );
        return default_;
    }
    return ret;
}


static struct input_event {
    struct timeval time;
    unsigned short type, code;
    unsigned int value;
} discard_event;
int main() {
    const u timeout = get_env_u("timeout", 15) * 1000;
    char low[32], high[32];
    {
        char *tmp;
        strncpy(low, (tmp = getenv("low"), tmp ? tmp : "5"), 30);
        strncpy(high, (tmp = getenv("high"), tmp ? tmp : "50"), 30);
    }
    fprintf(stderr, "running with low %s, high %s, timeout %lu\n", low, high, timeout/1000);
    size_t low_len = strlen(low), high_len = strlen(high);

    if (read(5, NULL, 0) != 0) {
        perror("reading from fd 5 (event source)");
        return -1;
    }
    if (write(6, high, high_len) < 0) {
        perror("writing to fd 6 (setting backlight high)");
        return -1;
    }
    struct pollfd event = { 5, POLLIN };
    while (1) {
        await_event:
        if (poll(&event, 1, timeout) == 0) {
            if (write(6, low, low_len) < 0) {
                perror("setting backlight low");
                return -1;
            }
        } else {
            while (poll(&event, 1, 1) != 0) {
                if (read(5, &discard_event, sizeof(discard_event)) < 0) {
                    perror("Reading event");
                    return -1;
                }
            }
            goto await_event;
        }

        await_wakeup:
        if (poll(&event, 1, 1<<30) == 0) {
            goto await_wakeup;
        } else {
            if (write(6, high, high_len) < 0) {
                perror("setting backlight high");
                return -1;
            }
        }
    }
    return 0;
}
