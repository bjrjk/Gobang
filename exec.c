#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <time.h>

#define INPUT 0 //Read End of Pipe
#define OUTPUT 1 //Write End of Pipe

static unsigned long long TimespecDiffInUs(const struct timespec * t1, const struct timespec * t2)
{
    struct timespec diff;
    if (t2->tv_nsec - t1->tv_nsec < 0) {
        diff.tv_sec  = t2->tv_sec - t1->tv_sec - 1;
        diff.tv_nsec = t2->tv_nsec - t1->tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2->tv_sec - t1->tv_sec;
        diff.tv_nsec = t2->tv_nsec - t1->tv_nsec;
    }
    return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}

static struct timespec getMonotonicClockTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1) {
        perror("clock_gettime failed");
        exit(1);
    }
    return time;
}

int createProcessWithRedirectedStdinAndStdout(
    const char * pathname, 
    int * stdinFd, int * stdoutFd,
    unsigned int alarmSecond
) {
    // Create stdin, stdout pipes
    int stdinFdArr[2], stdoutFdArr[2];
    if (pipe(stdinFdArr) == -1) {
        perror("pipe Failed\n");
        return 0;
    }
    if (pipe(stdoutFdArr) == -1) {
        perror("pipe Failed\n");
        return 0;
    }
    // Fork
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork Failed\n");
        return 0;
    } else if (pid == 0) { // Child Process
        // Close write end of stdin; read end of stdout
        close(stdinFdArr[OUTPUT]);
        close(stdoutFdArr[INPUT]);
        // dup2 pipe stdinFdArr[INPUT] & stdoutFdArr[OUTPUT] to stdin & stdout, respectively
        dup2(stdinFdArr[INPUT], 0);
        dup2(stdoutFdArr[OUTPUT], 1);
        // Alarm
        alarm(alarmSecond);
        // Exec
        int result;
        result = execl(pathname, pathname, NULL);
        // On exec failed
        perror("execvp failed\n");
        exit(1);
    } else { // Parent Process
        // Close read end of stdin; write end of stdout
        close(stdinFdArr[INPUT]);
        close(stdoutFdArr[OUTPUT]);
        // Write stdinFdArr[OUTPUT] & stdoutFdArr[INPUT] to stdinFd & stdoutFd, repectively
        *stdinFd = stdinFdArr[OUTPUT];
        *stdoutFd = stdoutFdArr[INPUT];
        return 1;
    }
}

int createProcessWithGivenStdinAndGetStdout(
    const char * pathname, 
    const char * stdinBuf, unsigned long stdinBufSize,
    char * stdoutBuf, unsigned long stdoutBufSize, unsigned long * stdoutReadSize,
    unsigned int alarmSecond, int prompt
) {
    int stdinFd, stdoutFd;
    // Create Process
    if (!createProcessWithRedirectedStdinAndStdout(pathname, &stdinFd, &stdoutFd, alarmSecond))
        return 0;
    // Write to subprocess's stdin
    if (write(stdinFd, stdinBuf, stdinBufSize) != stdinBufSize) {
        perror("Write error or doesn't write all buffer");
        return 0;
    }
    // Close subprocess's stdin pipe on write end
    if (close(stdinFd) == -1) {
        perror("close failed on stdinFd");
        return 0;
    }
    if (prompt) {
        struct timespec startTime = getMonotonicClockTime();
        while (1) {
            struct timespec currentTime = getMonotonicClockTime();
            unsigned long long timeDiff = TimespecDiffInUs(&startTime, &currentTime);
            printf("\rTime elapsed: %3lld.%06llds...", timeDiff / 1000000, timeDiff % 1000000);
            fflush(stdout);
            usleep(100000); // 0.1s
            // Check whether data is available
            if (poll(&(struct pollfd){ .fd = stdoutFd, .events = POLLIN }, 1, 0) == 1) {
                if ((*stdoutReadSize = read(stdoutFd, stdoutBuf, stdoutBufSize)) == -1) {
                    perror("read failed");
                    return 0;
                }
                break;
            }
        }
        printf("\r");
    } else {
        // Receive output from subprocess's stdout
        if ((*stdoutReadSize = read(stdoutFd, stdoutBuf, stdoutBufSize)) == -1) {
            perror("read failed");
            return 0;
        }
    }

    // Recycle zombie subprocess
    int retCode;
    if (wait(&retCode) == -1) {
        perror("wait failed");
        return 0;
    }
    return 1;
}