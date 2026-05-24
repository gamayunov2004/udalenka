#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void print_time_info(const char *name) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buf[16];
    strftime(buf, sizeof(buf), "%H:%M:%S", tm_info);
    printf("[%s] PID=%d, PPID=%d, pthread_id=%lu, time=%s\n",
           name, getpid(), getppid(), (unsigned long)pthread_self(), buf);
    fflush(stdout);
}

void *thread_func(void *arg) {
    const char *name = (const char *)arg;
    print_time_info(name);
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    print_time_info("main thread");

    pthread_create(&t1, NULL, thread_func, "thread 1");
    pthread_create(&t2, NULL, thread_func, "thread 2");

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Main thread: both child threads finished\n");
    return 0;
}
