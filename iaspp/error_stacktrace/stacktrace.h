#ifndef STACKTRACE_H
#define STACKTRACE_H

#include <stdlib.h>
#ifdef WINDOWS
#include <windows.h>
#elif LINUX
#include <err.h>
#include <stdint.h>
#include <signal.h>
#endif

static char const * icky_global_program_name;
int init_stack_trace(char * argv[]);

void set_signal_handler();
int addr2line(char const * const program_name, void const * const addr);

#ifdef WINDOWS
    LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS * ExceptionInfo);
    void windows_print_stacktrace(CONTEXT* context);

#elif LINUX


    #define MAX_STACK_FRAMES 64
    static uint8_t alternate_stack[SIGSTKSZ] = {};
    static void *stack_traces[MAX_STACK_FRAMES] = {};

    void posix_signal_handler(int sig, siginfo_t *siginfo, void *context);
    void posix_print_stack_trace();

#endif //win32 linux

#endif // STACKTRACE_H
