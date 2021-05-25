// author: xl69

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t child = -1;
int status = 0;

void handler (int signal);
void restart(char **argv);

int main(int argc, char** argv) {
    (void) argc;
    signal(SIGINT, handler);
    restart(argv);
}

void handler (int signal) {
    (void) signal;
    if (child != -1) {
        if (!kill(child, SIGINT)) {
            exit(0);
        }
    }
}

void restart(char **argv) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failure");
        exit(1);
    } else if (pid == 0) {
        if (execvp(argv[1], argv + 1) == -1) perror("Cannot execute the file");
    } else {
        child = pid;
        if (waitpid(pid, &status, 0) == -1) {
            perror("wait failure");
            exit(0);
        }
        if (WIFSIGNALED(status)) {
            if (WTERMSIG(status) == SIGSEGV) { // handle segfault signal
                restart(argv);
            }
        }
    }
}