#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define INPUT 0 //Read End of Pipe
#define OUTPUT 1 //Write End of Pipe

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
    unsigned int alarmSecond
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
    // Receive output from subprocess's stdout
    if ((*stdoutReadSize = read(stdoutFd, stdoutBuf, stdoutBufSize)) == -1) {
        perror("read failed");
        return 0;
    }
    // Recycle zombie subprocess
    int retCode;
    if (wait(&retCode) == -1) {
        perror("wait failed");
        return 0;
    }
    return 1;
}