#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define SEM_NAME "/gamayunov_lr8_named_sem"
#define LOG_FILE "/tmp/gamayunov_lr8_shared_log.txt"

static void print_time(FILE *f) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%H:%M:%S", tm_info);
    fprintf(f, "%s", buf);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s worker_name|cleanup\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "cleanup") == 0) {
        sem_unlink(SEM_NAME);
        unlink(LOG_FILE);
        printf("cleanup done\n");
        return 0;
    }

    const char *worker = argv[1];
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    printf("%s waits named semaphore\n", worker);
    sem_wait(sem);
    printf("%s entered critical section\n", worker);

    FILE *f = fopen(LOG_FILE, "a");
    if (!f) {
        perror("fopen");
        sem_post(sem);
        sem_close(sem);
        return 1;
    }

    for (int i = 1; i <= 5; i++) {
        fprintf(f, "%s line %d at ", worker, i);
        print_time(f);
        fprintf(f, "\n");
        fflush(f);
        usleep(200000);
    }
    fclose(f);

    printf("%s leaves critical section\n", worker);
    sem_post(sem);
    sem_close(sem);
    return 0;
}
