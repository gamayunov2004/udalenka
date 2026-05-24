#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THREADS 4
#define ITERATIONS 100000
#define PRODUCE_COUNT 5

static long long counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

static sem_t resource_sem;
static pthread_mutex_t inside_mutex = PTHREAD_MUTEX_INITIALIZER;
static int inside = 0;

static pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int item_ready = 0;
static int item_value = 0;
static int finished = 0;

void *mutex_worker(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&counter_mutex);
        counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    printf("mutex thread %ld finished\n", id);
    return NULL;
}

void run_mutex_demo(void) {
    pthread_t t[THREADS];
    counter = 0;
    printf("Mutex demo: %d threads, %d increments each\n", THREADS, ITERATIONS);
    for (long i = 0; i < THREADS; i++) {
        pthread_create(&t[i], NULL, mutex_worker, (void *)i);
    }
    for (int i = 0; i < THREADS; i++) {
        pthread_join(t[i], NULL);
    }
    printf("Expected counter = %d\n", THREADS * ITERATIONS);
    printf("Actual counter   = %lld\n", counter);
}

void *semaphore_worker(void *arg) {
    long id = (long)arg;
    printf("thread %ld waits for semaphore\n", id);
    sem_wait(&resource_sem);

    pthread_mutex_lock(&inside_mutex);
    inside++;
    printf("thread %ld entered resource, inside=%d\n", id, inside);
    pthread_mutex_unlock(&inside_mutex);

    sleep(1);

    pthread_mutex_lock(&inside_mutex);
    inside--;
    printf("thread %ld leaves resource, inside=%d\n", id, inside);
    pthread_mutex_unlock(&inside_mutex);

    sem_post(&resource_sem);
    return NULL;
}

void run_semaphore_demo(void) {
    pthread_t t[5];
    inside = 0;
    sem_init(&resource_sem, 0, 2);
    printf("Semaphore demo: only 2 threads may enter resource at the same time\n");
    for (long i = 0; i < 5; i++) {
        pthread_create(&t[i], NULL, semaphore_worker, (void *)i);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(t[i], NULL);
    }
    sem_destroy(&resource_sem);
}

void *producer(void *arg) {
    (void)arg;
    for (int i = 1; i <= PRODUCE_COUNT; i++) {
        pthread_mutex_lock(&cond_mutex);
        while (item_ready) {
            pthread_cond_wait(&cond, &cond_mutex);
        }
        item_value = i * 10;
        item_ready = 1;
        printf("producer created item %d\n", item_value);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&cond_mutex);
        sleep(1);
    }

    pthread_mutex_lock(&cond_mutex);
    finished = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&cond_mutex);
    return NULL;
}

void *consumer(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&cond_mutex);
        while (!item_ready && !finished) {
            pthread_cond_wait(&cond, &cond_mutex);
        }
        if (!item_ready && finished) {
            pthread_mutex_unlock(&cond_mutex);
            break;
        }
        printf("consumer received item %d\n", item_value);
        item_ready = 0;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&cond_mutex);
    }
    printf("consumer finished\n");
    return NULL;
}

void run_condition_demo(void) {
    pthread_t prod, cons;
    item_ready = 0;
    item_value = 0;
    finished = 0;
    printf("Condition variable demo: producer wakes consumer when item is ready\n");
    pthread_create(&cons, NULL, consumer, NULL);
    pthread_create(&prod, NULL, producer, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s mutex|sem|cond\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "mutex") == 0) {
        run_mutex_demo();
    } else if (strcmp(argv[1], "sem") == 0) {
        run_semaphore_demo();
    } else if (strcmp(argv[1], "cond") == 0) {
        run_condition_demo();
    } else {
        printf("Unknown mode: %s\n", argv[1]);
        return 1;
    }
    return 0;
}
