#include <iostream>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "board.h"
#include "movepanel.h"

static constexpr int targetWindowHeight = 1200;
static constexpr int targetWindowAspectNom = 5;
static constexpr int targetWindowAspectDenom = 4;
static constexpr unsigned int refreshTimeMs = 100;

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

    if (TTF_Init() < 0)
    {
        std::cerr << "TTF_Init failed, error: " << TTF_GetError() << "\n";
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    auto windowHeight = displayMode.h * 3 / 4;

    ctx.window = SDL_CreateWindow("Fatpup Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  windowHeight * targetWindowAspectNom / targetWindowAspectDenom,
                                  windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
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
    fatpup::Engine* engine = fatpup::Engine::Create("minimax");
    if (!engine)
    {
        std::cerr << "Can't create minimax engine, terminating...\n";
        return -1;
    }

    SDLContext ctx;
    if (InitSDL(ctx))
    {
        Board board(ctx.renderer, targetWindowHeight, targetWindowHeight, true);
        fatpup::Position initialPos;
        initialPos.setInitial();
        board.SetPosition(initialPos);
        engine->SetPosition(initialPos);
        board.SetEngine(engine);

        MovePanel movePanel(ctx.renderer, targetWindowHeight,
                            targetWindowHeight * (targetWindowAspectNom - targetWindowAspectDenom) / targetWindowAspectDenom,
                            targetWindowHeight);
        board.SetMovePanel(&movePanel);

        int currWindowWidth = 0, currWindowHeight = 0;
        SDL_GetWindowSize(ctx.window, &currWindowWidth, &currWindowHeight);
        float renderScale = 1.0f;
        float pixelScale = 1.0f;
        bool windowSizeChanged = true;

        unsigned int nextRefreshTimeMs = 0;

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
                    case SDL_MOUSEBUTTONDOWN:
                        if (e.button.button == SDL_BUTTON_LEFT)
                            board.OnClick((int)(e.button.x / pixelScale), (int)(e.button.y / pixelScale));
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
                pixelScale = (float)currWindowHeight / (float)targetWindowHeight;

                int highDpiWidth = 0, highDpiHeight = 0;
                SDL_GetRendererOutputSize(ctx.renderer, &highDpiWidth, &highDpiHeight);
                renderScale = (float)highDpiHeight / (float)targetWindowHeight;
            }

            unsigned int currTimeMs = SDL_GetTicks();
            if (!SDL_TICKS_PASSED(currTimeMs, nextRefreshTimeMs))
                SDL_Delay(refreshTimeMs / 4);
            else
            {
                nextRefreshTimeMs = currTimeMs + refreshTimeMs;

                SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, 255);
                SDL_RenderClear(ctx.renderer);

                SDL_RenderSetScale(ctx.renderer, renderScale, renderScale);
                board.Render();
                movePanel.Render();
                SDL_RenderSetScale(ctx.renderer, 1.0f, 1.0f);

                SDL_RenderPresent(ctx.renderer);
            }
        }
    }

    delete engine;

    if (ctx.renderer)
        SDL_DestroyRenderer(ctx.renderer);
    if (ctx.window)
        SDL_DestroyWindow(ctx.window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
