#pragma once

extern "C"
int createProcessWithRedirectedStdinAndStdout(
    const char * pathname, 
    int * stdinFd, int * stdoutFd,
    unsigned int alarmSecond
);

extern "C"
int createProcessWithGivenStdinAndGetStdout(
    const char * pathname, 
    const char * stdinBuf, unsigned long stdinBufSize,
    char * stdoutBuf, unsigned long stdoutBufSize, unsigned long * stdoutReadSize,
    unsigned int alarmSecond, int prompt
);