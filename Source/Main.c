#include "stdio.h"

// Core
#include "CoreTypes.h"
#include "CoreLogging.h"

// SDL
// TODO: Dynamic link
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

enum RAYTRACER_LOG_CATEGORIES
{ 
    RAYTRACER_LOG_INIT
};

i32 main(i32 argc, char** argv)
{
    // TODO: Replace malloc implementation
    // SDL_SetMemoryFunctions()


    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) != 0) {
        CORE_LOG_ERROR(RAYTRACER_LOG_INIT, "SDL_Init Failed (%s)", SDL_GetError());
        return -1;
    }

    SDL_Window *SDLWindow = NULL;
    SDL_Renderer *SDLRenderer = NULL;

    if (SDL_CreateWindowAndRenderer(640, 480, 0, &SDLWindow, &SDLRenderer) != 0) {
        CORE_LOG_ERROR(RAYTRACER_LOG_INIT, "SDL_CreateWindowAndRenderer Failed (%s)", SDL_GetError());
        SDL_Quit();
        return -1;
    }


    SDL_SetWindowTitle(SDLWindow, "Raytracer");

    bool AppIsRunning = true;
    do {
        SDL_Event CurrentEvent;
        while (SDL_PollEvent(&CurrentEvent)) {
            switch (CurrentEvent.type)
            {
                case SDL_EVENT_QUIT:
                {
                    AppIsRunning = false;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(SDLRenderer, 80, 80, 80, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(SDLRenderer);
        SDL_RenderPresent(SDLRenderer);
    } while (AppIsRunning);

    SDL_DestroyRenderer(SDLRenderer);
    SDL_DestroyWindow(SDLWindow);

    SDL_Quit();
    return 0;
}