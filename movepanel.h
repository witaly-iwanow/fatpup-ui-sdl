#ifndef FATPUP_UI_MOVEPANEL_H
#define FATPUP_UI_MOVEPANEL_H

#include <map>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "fatpup/position.h"

class MovePanel
{
public:
    explicit MovePanel(SDL_Renderer* renderer, int xOff, int width, int height);
    ~MovePanel();

    void SetPosition(const fatpup::Position& pos);
    void Move(fatpup::Move move);

    void Render();

private:
    void UpdateTextures();
    void UpdateTexture(SDL_Surface* surface, SDL_Texture** texture, SDL_Rect* textureRect); // releases surface too

    SDL_Renderer* _renderer = nullptr;

    fatpup::Position _position;
    fatpup::Position::State _positionState;
    bool _positionUpdated = true;

    SDL_Rect _backgroundRect;
    TTF_Font* _font = nullptr;

    enum
    {
        White = 0,
        Black = 1
    };
    std::string _movesString[2];
    SDL_Texture* _movesTexture[2] = { nullptr, nullptr };
    SDL_Rect _movesTextureRect[2];

    SDL_Texture* _turnIndicatorTexture[2] = { nullptr, nullptr };
    SDL_Rect _turnIndicatorTextureRect[2];

    SDL_Texture* _winDrawIndicatorTexture = nullptr;
    SDL_Rect _winDrawIndicatorTextureRect;
};

#endif // #define FATPUP_UI_MOVEPANEL_H
