#ifndef IASPP_GLOBAL_H
#define IASPP_GLOBAL_H

#ifdef WINDOWS
    #ifdef IASPP_LIBRARY
        #define IASPP_PUBLIC __declspec(dllexport)
    #else
        #define IASPP_PUBLIC __declspec(dllimport)
    #endif
    #define DEPRECATED(func) __declspec(deprecated) func
#elif LINUX
    #define IASPP_PUBLIC
    #define DEPRECATED(func) func __attribute__ ((deprecated))
#endif

#endif // IASPP_GLOBAL_H
