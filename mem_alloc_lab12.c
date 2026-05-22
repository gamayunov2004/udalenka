cat > mem_alloc_lab12.c <<'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define BLOCK_SIZE (512 * 1024)
#define SLEEP_NS 500000000L

static volatile sig_atomic_t running = 1;

void stop_handler(int sig) {
    (void)sig;
    running = 0;
}

void sleep_short(void) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = SLEEP_NS;
    nanosleep(&ts, NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s P1|P2\n", argv[0]);
        return 1;
    }

    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);

    printf("PID: %d\n", getpid());
    printf("Mode: %s\n", argv[1]);
    fflush(stdout);

    void *previous = NULL;
    void **blocks = NULL;
    size_t count = 0;
    size_t capacity = 0;
    unsigned long iteration = 0;

    while (running) {
        void *ptr = malloc(BLOCK_SIZE);
        if (ptr == NULL) {
            perror("malloc");
            break;
        }

        memset(ptr, 1, BLOCK_SIZE);
        iteration++;

        if (strcmp(argv[1], "P1") == 0) {
            free(previous);
            previous = ptr;
        } else if (strcmp(argv[1], "P2") == 0) {
            if (iteration % 5 == 0) {
                free(ptr);
            } else {
                if (count == capacity) {
                    capacity = capacity == 0 ? 16 : capacity * 2;
                    void **tmp = realloc(blocks, capacity * sizeof(void *));
                    if (tmp == NULL) {
                        perror("realloc");
                        free(ptr);
                        break;
                    }
                    blocks = tmp;
                }
                blocks[count++] = ptr;
            }
        } else {
            printf("Unknown mode. Use P1 or P2.\n");
            free(ptr);
            free(previous);
            free(blocks);
            return 1;
        }

        printf("iteration=%lu, saved_blocks=%zu\n", iteration, count);
        fflush(stdout);
        sleep_short();
    }

    free(previous);
    for (size_t i = 0; i < count; i++) {
        free(blocks[i]);
    }
    free(blocks);

    printf("Memory released. Program finished.\n");
    return 0;
}
EOF
