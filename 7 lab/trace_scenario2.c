#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        execlp("echo", "echo", "Hello world!", NULL);
        perror("execlp");
        return 2;
    }
    printf("Parent says: this is not Hello world!\n");
    waitpid(pid, NULL, 0);
    return 0;
}
