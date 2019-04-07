#include "iaspp.h"

#include "error_stacktrace/stacktrace.h"

std::string iaspp::to_string(bool b)
{
    return b ? "true" : "false";
}

int iaspp::IASPP_INIT(char *argv[])
{
    setlocale(LC_ALL, "Portuguese");
    //Start up SDL and make sure it went ok
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        trace("SDL - init fail");
        return EXIT_FAILURE;
    }

    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initedFlags = IMG_Init(flags);
    if((initedFlags & flags) != flags)
    {
        trace("SDL - init image fail");
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if(TTF_Init() != 0)
    {
        trace("SDL - init ttf fail");
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if(init_stack_trace(argv) != 0)
    {
        trace("ASPP - stacktrace init fail");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void iaspp::IASPP_QUIT()
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
