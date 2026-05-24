#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void print_info(const char *name) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buf[16];
    strftime(buf, sizeof(buf), "%H:%M:%S", tm_info);
    printf("[%s] PID=%d, PPID=%d, time=%s\n", name, getpid(), getppid(), buf);
    fflush(stdout);
}

int main(void) {
    print_info("parent before fork");

    pid_t child1 = fork();
    if (child1 < 0) {
        perror("fork child1");
        return 1;
    }
    if (child1 == 0) {
        print_info("child 1");
        printf("Child 1 task, variant 6: home directory sorted by access time\n");
        system("ls -ltu --time=atime $HOME | head -15");
        sleep(5);
        return 0;
    }

    pid_t child2 = fork();
    if (child2 < 0) {
        perror("fork child2");
        return 1;
    }
    if (child2 == 0) {
        print_info("child 2");
        sleep(5);
        return 0;
    }

    print_info("parent after fork");
    printf("Parent: child1 PID=%d, child2 PID=%d\n", child1, child2);
    printf("Parent runs ps -x to find created processes:\n");
    system("ps -x | grep -E 'lab7_fork|PID|ps' | grep -v grep");

    int status;
    waitpid(child1, &status, 0);
    printf("Parent: child1 finished, status=%d\n", status);
    waitpid(child2, &status, 0);
    printf("Parent: child2 finished, status=%d\n", status);
    return 0;
}
