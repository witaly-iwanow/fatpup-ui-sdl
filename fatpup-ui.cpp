#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

#include "board.h"

static constexpr int targetWindowHeight = 800;
static constexpr int targetWindowAspectNom = 5;
static constexpr int targetWindowAspectDenom = 4;

struct SDLContext
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

bool InitSDL(SDLContext& ctx)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL_Init failed, error: " << SDL_GetError() << "\n";
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cerr << "IMG_Init failed, error: " << IMG_GetError() << "\n";
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    ctx.window = SDL_CreateWindow("Fatpup Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  targetWindowHeight * targetWindowAspectNom / targetWindowAspectDenom,
                                  targetWindowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (!ctx.window)
    {
        std::cerr << "SDL_CreateWindow failed, error: " << SDL_GetError() << "\n";
        return false;
    }

    ctx.renderer = SDL_CreateRenderer(ctx.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ctx.renderer)
    {
        std::cerr << "SDL_CreateRenderer failed, error: " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    SDLContext ctx;
    if (InitSDL(ctx))
    {
        Board board(ctx.renderer, targetWindowHeight, targetWindowHeight, true);
        fatpup::Position initialPos;
        initialPos.setInitial();
        board.SetPosition(initialPos);

        int currWindowWidth = 0, currWindowHeight = 0;
        SDL_GetWindowSize(ctx.window, &currWindowWidth, &currWindowHeight);
        float renderScale = 1.0f;

        SDL_Event e;
        bool quit = false;
        while (!quit)
        {
            bool windowSizeChanged = false;
            while (SDL_PollEvent(&e) > 0)
            {
                switch (e.type)
                {
                    case SDL_QUIT:
                        quit = true;
                        break;
                    case SDL_WINDOWEVENT:
                    {
                        switch(e.window.event)
                        {
                            case SDL_WINDOWEVENT_RESIZED:
                                windowSizeChanged = true;
                                break;
                        }
                    }
                }

                if (windowSizeChanged)
                {
                    windowSizeChanged = false;

                    int width = 0, height = 0;
                    SDL_GetWindowSize(ctx.window, &width, &height);

                    width -= (width % 5);
                    height -= (height % 4);

                    if (width < 80)
                        width = 80;
                    if (height < 64)
                        height = 64;

                    if (std::abs(width - currWindowWidth) > std::abs(height - currWindowHeight))
                        height = width * 4 / 5;
                    else
                        width = height * 5 / 4;

                    SDL_SetWindowSize(ctx.window, width, height);
                    currWindowWidth = width;
                    currWindowHeight = height;
                    renderScale = (float)currWindowHeight / (float)targetWindowHeight;
                }

                SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, 255);
                SDL_RenderClear(ctx.renderer);

                SDL_RenderSetScale(ctx.renderer, renderScale, renderScale);
                board.Render();
                SDL_RenderSetScale(ctx.renderer, 1.0f, 1.0f);

                SDL_RenderPresent(ctx.renderer);
            }
        }
    }

    if (ctx.renderer)
        SDL_DestroyRenderer(ctx.renderer);
    if (ctx.window)
        SDL_DestroyWindow(ctx.window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
