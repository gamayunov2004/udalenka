#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(void)
{
    FILE *f = fopen("/tmp/lr5_own_program.log", "w");
    if (f == NULL) {
        perror("fopen");
        return 1;
    }

    for (int i = 1; i <= 62; i++) {
        time_t now = time(NULL);
        fprintf(f, "iteration %d time %ld\n", i, (long)now);
        fflush(f);
        sleep(1);
    }

    fclose(f);
    return 0;
}
