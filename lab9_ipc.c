#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#define N 4
#define WORKERS 4
#define SHM_NAME "/gamayunov_lab9_shm"

typedef struct {
    int col;
    long long coef;
    long long minor[3][3];
} Task;

typedef struct {
    int col;
    long long partial;
} Result;

typedef struct {
    Task tasks[WORKERS];
    Result results[WORKERS];
} SharedData;

static long long det3(long long a[3][3]) {
    long long d = 0;
    d += a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]);
    d -= a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0]);
    d += a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
    return d;
}

static void make_task(long long matrix[N][N], int col, Task *task) {
    task->col = col;
    task->coef = matrix[0][col];
    int mi = 0;
    for (int i = 1; i < N; i++) {
        int mj = 0;
        for (int j = 0; j < N; j++) {
            if (j == col) continue;
            task->minor[mi][mj++] = matrix[i][j];
        }
        mi++;
    }
}

static Result solve_task(Task *task) {
    Result r;
    r.col = task->col;
    long long sign = (task->col % 2 == 0) ? 1 : -1;
    r.partial = sign * task->coef * det3(task->minor);
    return r;
}

static long long sum_results(Result results[WORKERS]) {
    long long sum = 0;
    for (int i = 0; i < WORKERS; i++) sum += results[i].partial;
    return sum;
}

static long long run_pipe_once(long long matrix[N][N]) {
    int to_child[WORKERS][2];
    int from_child[WORKERS][2];
    pid_t pids[WORKERS];
    Result results[WORKERS];

    for (int i = 0; i < WORKERS; i++) {
        if (pipe(to_child[i]) == -1 || pipe(from_child[i]) == -1) {
            perror("pipe"); exit(1);
        }
        pids[i] = fork();
        if (pids[i] == -1) { perror("fork"); exit(1); }
        if (pids[i] == 0) {
            close(to_child[i][1]);
            close(from_child[i][0]);
            Task t;
            if (read(to_child[i][0], &t, sizeof(t)) != sizeof(t)) exit(2);
            Result r = solve_task(&t);
            if (write(from_child[i][1], &r, sizeof(r)) != sizeof(r)) exit(3);
            close(to_child[i][0]);
            close(from_child[i][1]);
            exit(0);
        }
        close(to_child[i][0]);
        close(from_child[i][1]);
    }

    for (int i = 0; i < WORKERS; i++) {
        Task t;
        make_task(matrix, i, &t);
        if (write(to_child[i][1], &t, sizeof(t)) != sizeof(t)) { perror("write"); exit(1); }
        close(to_child[i][1]);
    }
    for (int i = 0; i < WORKERS; i++) {
        if (read(from_child[i][0], &results[i], sizeof(results[i])) != sizeof(results[i])) {
            perror("read"); exit(1);
        }
        close(from_child[i][0]);
    }
    for (int i = 0; i < WORKERS; i++) waitpid(pids[i], NULL, 0);
    return sum_results(results);
}

static long long run_shm_once(long long matrix[N][N]) {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (fd == -1) { perror("shm_open"); exit(1); }
    if (ftruncate(fd, sizeof(SharedData)) == -1) { perror("ftruncate"); exit(1); }
    SharedData *data = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) { perror("mmap"); exit(1); }

    for (int i = 0; i < WORKERS; i++) make_task(matrix, i, &data->tasks[i]);

    pid_t pids[WORKERS];
    for (int i = 0; i < WORKERS; i++) {
        pids[i] = fork();
        if (pids[i] == -1) { perror("fork"); exit(1); }
        if (pids[i] == 0) {
            data->results[i] = solve_task(&data->tasks[i]);
            munmap(data, sizeof(SharedData));
            close(fd);
            exit(0);
        }
    }
    for (int i = 0; i < WORKERS; i++) waitpid(pids[i], NULL, 0);
    long long det = sum_results(data->results);

    munmap(data, sizeof(SharedData));
    close(fd);
    shm_unlink(SHM_NAME);
    return det;
}

static long long run_socket_once(long long matrix[N][N]) {
    int sv[WORKERS][2];
    pid_t pids[WORKERS];
    Result results[WORKERS];

    for (int i = 0; i < WORKERS; i++) {
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv[i]) == -1) {
            perror("socketpair"); exit(1);
        }
        pids[i] = fork();
        if (pids[i] == -1) { perror("fork"); exit(1); }
        if (pids[i] == 0) {
            close(sv[i][0]);
            Task t;
            if (read(sv[i][1], &t, sizeof(t)) != sizeof(t)) exit(2);
            Result r = solve_task(&t);
            if (write(sv[i][1], &r, sizeof(r)) != sizeof(r)) exit(3);
            close(sv[i][1]);
            exit(0);
        }
        close(sv[i][1]);
    }

    for (int i = 0; i < WORKERS; i++) {
        Task t;
        make_task(matrix, i, &t);
        if (write(sv[i][0], &t, sizeof(t)) != sizeof(t)) { perror("write socket"); exit(1); }
    }
    for (int i = 0; i < WORKERS; i++) {
        if (read(sv[i][0], &results[i], sizeof(results[i])) != sizeof(results[i])) {
            perror("read socket"); exit(1);
        }
        close(sv[i][0]);
    }
    for (int i = 0; i < WORKERS; i++) waitpid(pids[i], NULL, 0);
    return sum_results(results);
}

static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

static long get_rss_kb(void) {
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) return -1;
    char line[256];
    long rss = -1;
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "VmRSS: %ld kB", &rss) == 1) break;
    }
    fclose(f);
    return rss;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s pipe|shm|socket [repeats]\n", argv[0]);
        return 1;
    }
    int repeats = 1;
    if (argc >= 3) repeats = atoi(argv[2]);
    if (repeats < 1) repeats = 1;

    long long matrix[N][N] = {
        {3, 2, 0, 1},
        {4, 0, 1, 2},
        {3, 0, 2, 1},
        {9, 2, 3, 1}
    };

    printf("Matrix 4x4:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) printf("%4lld", matrix[i][j]);
        printf("\n");
    }
    fflush(stdout);

    double start = now_ms();
    long long det = 0;
    for (int i = 0; i < repeats; i++) {
        if (strcmp(argv[1], "pipe") == 0) det = run_pipe_once(matrix);
        else if (strcmp(argv[1], "shm") == 0) det = run_shm_once(matrix);
        else if (strcmp(argv[1], "socket") == 0) det = run_socket_once(matrix);
        else {
            fprintf(stderr, "Unknown mode: %s\n", argv[1]);
            return 1;
        }
    }
    double finish = now_ms();

    size_t task_bytes = sizeof(Task) * WORKERS;
    size_t result_bytes = sizeof(Result) * WORKERS;
    long messages;
    size_t ipc_bytes;

    if (strcmp(argv[1], "shm") == 0) {
        messages = WORKERS; // child processes write results into shared memory
        ipc_bytes = sizeof(SharedData);
    } else {
        messages = WORKERS * 2; // tasks to children + results to parent
        ipc_bytes = task_bytes + result_bytes;
    }

    printf("Mode: %s\n", argv[1]);
    printf("Repeats: %d\n", repeats);
    printf("Determinant: %lld\n", det);
    printf("Workers: %d\n", WORKERS);
    printf("Messages per run: %ld\n", messages);
    printf("Approx IPC bytes per run: %zu\n", ipc_bytes);
    printf("Process VmRSS now: %ld kB\n", get_rss_kb());
    printf("Elapsed: %.3f ms\n", finish - start);
    return 0;
}
