#include <iostream>

#include "SDL.h" 

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Fatpup Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&e) > 0)
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
            }

            SDL_UpdateWindowSurface(window);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
