#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#define NUM_CHILDREN 3

pid_t children[NUM_CHILDREN];
int current = 0;

void scheduler(int sig) {
    kill(children[current], SIGSTOP);
    current = (current + 1) % NUM_CHILDREN;
    kill(children[current], SIGCONT);
}

int main() {
    struct itimerval timer;

    printf("Starting simple time-sharing simulation...\n");

    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            while (1) {
                printf("Child %d (PID %d) is running...\n", i, getpid());
                usleep(200000);
            }
        } else {
            children[i] = pid;
            kill(pid, SIGSTOP);
        }
    }

    signal(SIGALRM, scheduler);

    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);

    kill(children[current], SIGCONT);

    while (1) pause();

    return 0;
}
