#ifndef IASPP_H
#define IASPP_H

#ifdef WINDOWS
    #include "SDL.h"
    #include "SDL_image.h"
    #include "SDL_ttf.h"
#elif LINUX
    #include "SDL2/SDL.h"
    #include "SDL2/SDL_image.h"
    #include "SDL2/SDL_ttf.h"
#endif

#include <string>
#include <iostream>

#include "iaspp_global.h"

namespace iaspp {
    int IASPP_PUBLIC IASPP_INIT(char *argv[]);
    void IASPP_PUBLIC IASPP_QUIT();

    std::string to_string(bool b);
    template<typename T>
    T reduce(T from, T diff){
        return from < 0 ? (from + diff) : (from - diff);
    }

    template <typename T>
    void trace(T t){
        std::cout << t << std::endl;
    }

    template <typename T, typename... Args>
    void trace(T t, Args... args){
        std::cout << t << " ";
        trace(args...);
    }

    #define aspp_throw(ARG1) iaspp::trace("Error in file:", __FILE__, "in line", __LINE__, "at function", __func__, "(), with message:",ARG1)
}

#endif // IASPP_H
