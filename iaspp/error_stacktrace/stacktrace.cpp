#include "stacktrace.h"

#include <stdio.h>
#include <signal.h>
#include <assert.h>

#ifdef WINDOWS
#include <imagehlp.h>
#elif LINUX
#include <execinfo.h>
#endif

int init_stack_trace(char *argv[])
{
    /* store off program path so we can use it later */
    icky_global_program_name = argv[0];

    set_signal_handler();

    return EXIT_SUCCESS;
}


void set_signal_handler()
{
#ifdef LINUX
    /* setup alternate stack */
    {
        stack_t ss = {};
        /* malloc is usually used here, I'm not 100% sure my static allocation
           is valid but it seems to work just fine. */
        ss.ss_sp = (void*)alternate_stack;
        ss.ss_size = SIGSTKSZ;
        ss.ss_flags = 0;

        if (sigaltstack(&ss, nullptr) != 0) { err(1, "sigaltstack"); }
    }

    /* register our signal handlers */
    {
        struct sigaction sig_action = {};
        sig_action.sa_sigaction = posix_signal_handler;
        sigemptyset(&sig_action.sa_mask);

        sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;

        if (sigaction(SIGSEGV, &sig_action, nullptr) != 0) { err(1, "sigaction"); }
        if (sigaction(SIGFPE,  &sig_action, nullptr) != 0) { err(1, "sigaction"); }
        if (sigaction(SIGINT,  &sig_action, nullptr) != 0) { err(1, "sigaction"); }
        if (sigaction(SIGILL,  &sig_action, nullptr) != 0) { err(1, "sigaction"); }
        if (sigaction(SIGTERM, &sig_action, nullptr) != 0) { err(1, "sigaction"); }
        if (sigaction(SIGABRT, &sig_action, nullptr) != 0) { err(1, "sigaction"); }
    }
#elif defined WINDOWS
    SetUnhandledExceptionFilter(windows_exception_handler);
#endif
}

#ifdef WINDOWS
LONG windows_exception_handler(EXCEPTION_POINTERS *ExceptionInfo)
{
    switch(ExceptionInfo->ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        fputs("Error: EXCEPTION_ACCESS_VIOLATION\n", stderr);
        break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        fputs("Error: EXCEPTION_ARRAY_BOUNDS_EXCEEDED\n", stderr);
        break;
    case EXCEPTION_BREAKPOINT:
        fputs("Error: EXCEPTION_BREAKPOINT\n", stderr);
        break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        fputs("Error: EXCEPTION_DATATYPE_MISALIGNMENT\n", stderr);
        break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        fputs("Error: EXCEPTION_FLT_DENORMAL_OPERAND\n", stderr);
        break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        fputs("Error: EXCEPTION_FLT_DIVIDE_BY_ZERO\n", stderr);
        break;
    case EXCEPTION_FLT_INEXACT_RESULT:
        fputs("Error: EXCEPTION_FLT_INEXACT_RESULT\n", stderr);
        break;
    case EXCEPTION_FLT_INVALID_OPERATION:
        fputs("Error: EXCEPTION_FLT_INVALID_OPERATION\n", stderr);
        break;
    case EXCEPTION_FLT_OVERFLOW:
        fputs("Error: EXCEPTION_FLT_OVERFLOW\n", stderr);
        break;
    case EXCEPTION_FLT_STACK_CHECK:
        fputs("Error: EXCEPTION_FLT_STACK_CHECK\n", stderr);
        break;
    case EXCEPTION_FLT_UNDERFLOW:
        fputs("Error: EXCEPTION_FLT_UNDERFLOW\n", stderr);
        break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        fputs("Error: EXCEPTION_ILLEGAL_INSTRUCTION\n", stderr);
        break;
    case EXCEPTION_IN_PAGE_ERROR:
        fputs("Error: EXCEPTION_IN_PAGE_ERROR\n", stderr);
        break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        fputs("Error: EXCEPTION_INT_DIVIDE_BY_ZERO\n", stderr);
        break;
    case EXCEPTION_INT_OVERFLOW:
        fputs("Error: EXCEPTION_INT_OVERFLOW\n", stderr);
        break;
    case EXCEPTION_INVALID_DISPOSITION:
        fputs("Error: EXCEPTION_INVALID_DISPOSITION\n", stderr);
        break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        fputs("Error: EXCEPTION_NONCONTINUABLE_EXCEPTION\n", stderr);
        break;
    case EXCEPTION_PRIV_INSTRUCTION:
        fputs("Error: EXCEPTION_PRIV_INSTRUCTION\n", stderr);
        break;
    case EXCEPTION_SINGLE_STEP:
        fputs("Error: EXCEPTION_SINGLE_STEP\n", stderr);
        break;
    case EXCEPTION_STACK_OVERFLOW:
        fputs("Error: EXCEPTION_STACK_OVERFLOW\n", stderr);
        break;
    default:
        fputs("Error: Unrecognized Exception\n", stderr);
        break;
    }
    fflush(stderr);
    /* If this is a stack overflow then we can't walk the stack, so just show
        where the error happened */
    if (EXCEPTION_STACK_OVERFLOW != ExceptionInfo->ExceptionRecord->ExceptionCode)
    {
        windows_print_stacktrace(ExceptionInfo->ContextRecord);
    }
    else
    {
        addr2line(icky_global_program_name, (void*)ExceptionInfo->ContextRecord->Eip);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

void windows_print_stacktrace(CONTEXT *context)
{
    SymInitialize(GetCurrentProcess(), 0, true);

    STACKFRAME frame = { 0 };

    /* setup initial stack frame */
    frame.AddrPC.Offset         = context->Eip;
    frame.AddrPC.Mode           = AddrModeFlat;
    frame.AddrStack.Offset      = context->Esp;
    frame.AddrStack.Mode        = AddrModeFlat;
    frame.AddrFrame.Offset      = context->Ebp;
    frame.AddrFrame.Mode        = AddrModeFlat;

    while (StackWalk(IMAGE_FILE_MACHINE_I386 ,
                     GetCurrentProcess(),
                     GetCurrentThread(),
                     &frame,
                     context,
                     0,
                     SymFunctionTableAccess,
                     SymGetModuleBase,
                     0 ) )
    {
        addr2line(icky_global_program_name, (void*)frame.AddrPC.Offset);
    }

    SymCleanup( GetCurrentProcess() );
}
#elif LINUX
void posix_signal_handler(int sig, siginfo_t *siginfo, void *context)
{
    (void)context;
    switch(sig)
    {
    case SIGSEGV:
        fputs("Caught SIGSEGV: Segmentation Fault\n", stderr);
        break;
    case SIGINT:
        fputs("Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n",
              stderr);
        break;
    case SIGFPE:
        switch(siginfo->si_code)
        {
        case FPE_INTDIV:
            fputs("Caught SIGFPE: (integer divide by zero)\n", stderr);
            break;
        case FPE_INTOVF:
            fputs("Caught SIGFPE: (integer overflow)\n", stderr);
            break;
        case FPE_FLTDIV:
            fputs("Caught SIGFPE: (floating-point divide by zero)\n", stderr);
            break;
        case FPE_FLTOVF:
            fputs("Caught SIGFPE: (floating-point overflow)\n", stderr);
            break;
        case FPE_FLTUND:
            fputs("Caught SIGFPE: (floating-point underflow)\n", stderr);
            break;
        case FPE_FLTRES:
            fputs("Caught SIGFPE: (floating-point inexact result)\n", stderr);
            break;
        case FPE_FLTINV:
            fputs("Caught SIGFPE: (floating-point invalid operation)\n", stderr);
            break;
        case FPE_FLTSUB:
            fputs("Caught SIGFPE: (subscript out of range)\n", stderr);
            break;
        default:
            fputs("Caught SIGFPE: Arithmetic Exception\n", stderr);
            break;
        }
        break;
    case SIGILL:
        switch(siginfo->si_code)
        {
        case ILL_ILLOPC:
            fputs("Caught SIGILL: (illegal opcode)\n", stderr);
            break;
        case ILL_ILLOPN:
            fputs("Caught SIGILL: (illegal operand)\n", stderr);
            break;
        case ILL_ILLADR:
            fputs("Caught SIGILL: (illegal addressing mode)\n", stderr);
            break;
        case ILL_ILLTRP:
            fputs("Caught SIGILL: (illegal trap)\n", stderr);
            break;
        case ILL_PRVOPC:
            fputs("Caught SIGILL: (privileged opcode)\n", stderr);
            break;
        case ILL_PRVREG:
            fputs("Caught SIGILL: (privileged register)\n", stderr);
            break;
        case ILL_COPROC:
            fputs("Caught SIGILL: (coprocessor error)\n", stderr);
            break;
        case ILL_BADSTK:
            fputs("Caught SIGILL: (internal stack error)\n", stderr);
            break;
        default:
            fputs("Caught SIGILL: Illegal Instruction\n", stderr);
            break;
        }
        break;
    case SIGTERM:
        fputs("Caught SIGTERM: a termination request was sent to the program\n",
              stderr);
        break;
    case SIGABRT:
        fputs("Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
        break;
    default:
        break;
    }
    posix_print_stack_trace();
    _Exit(1);
}

void posix_print_stack_trace()
{
    int i, trace_size = 0;
    char **messages = (char **)nullptr;

    trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
    messages = backtrace_symbols(stack_traces, trace_size);

    /* skip the first couple stack frames (as they are this function and
         our handler) and also skip the last frame as it's (always?) junk. */
    // for (i = 3; i < (trace_size - 1); ++i)
    // we'll use this for now so you can see what's going on
    for (i = 0; i < trace_size; ++i)
    {
        if (addr2line(icky_global_program_name, stack_traces[i]) != 0)
        {
            printf("  error determining line # for: %s\n", messages[i]);
        }

    }
    if (messages) { free(messages); }
}

//uint8_t alternate_stack[SIGSTKSZ] = {};
//void *stack_traces[MAX_STACK_FRAMES] = {};
#endif //win32 linux

int addr2line(const char * const program_name, const void * const addr)
{
    char addr2line_cmd[512] = {0};

    /* have addr2line map the address to the relent line in the code */
    sprintf(addr2line_cmd,"addr2line -f -p -e %.256s %p", program_name, addr);

    /* This will print a nicely formatted string specifying the
         function and source line of the address */
    return system(addr2line_cmd);
}
