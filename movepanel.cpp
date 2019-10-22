#include <iostream>

#include "colors.h"

#include "movepanel.h"

MovePanel::MovePanel(SDL_Renderer* renderer, int xOff, int width, int height):
    _renderer(renderer)
{
    const int ptSizePriming = 32;
    if (!(_font = TTF_OpenFont("resources/Lekton-Bold.ttf", ptSizePriming)))
    {
        std::cerr << "Can't load font from resources/Lekton-Bold.ttf\n";
        return;
    }

    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(_font, "Ng1-f3+", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    int ptSizeTarget = ptSizePriming * width * 6 / (texW * 16);
    if (ptSizeTarget != ptSizePriming)
    {
        TTF_CloseFont(_font);
        _font = TTF_OpenFont("resources/Lekton-Bold.ttf", ptSizeTarget);

        surface = TTF_RenderText_Solid(_font, "Ng1-f3+", color);
        texture = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    _backgroundRect.x = xOff;
    _backgroundRect.y = 0;
    _backgroundRect.w = width;
    _backgroundRect.h = height;

    _movesTextureRect[White].x = _backgroundRect.x + _backgroundRect.w / 32;
    _movesTextureRect[White].y = texH + texH / 2;
    _movesTextureRect[Black].x = _movesTextureRect[White].x + _backgroundRect.w / 2;
    _movesTextureRect[Black].y = _movesTextureRect[White].y;

    _winDrawIndicatorTextureRect.x = _movesTextureRect[White].x;
    _winDrawIndicatorTextureRect.y = _backgroundRect.h - texH - texH / 4;

    _turnIndicatorTextureRect[White].x = _turnIndicatorTextureRect[Black].x = _movesTextureRect[White].x;
    _turnIndicatorTextureRect[White].y = _turnIndicatorTextureRect[Black].y = _backgroundRect.w / 32;

    SDL_Color colors[] = {{ 255, 255, 255, 255 }, { 0, 0, 0, 255 }};
    for (int c = White; c <= Black; ++c)
    {
        SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(_font, (c ? "Black turn" : "White turn"), colors[c], _backgroundRect.w);
        UpdateTexture(surface, &_turnIndicatorTexture[c], &_turnIndicatorTextureRect[c]);
    }

    _position.setInitial();
}

MovePanel::~MovePanel()
{
    for (int c = White; c <= Black; ++c)
    {
        if (_movesTexture[c])
            SDL_DestroyTexture(_movesTexture[c]);

        if (_turnIndicatorTexture[c])
            SDL_DestroyTexture(_turnIndicatorTexture[c]);
    }

    if (_winDrawIndicatorTexture)
        SDL_DestroyTexture(_winDrawIndicatorTexture);

    if (_font)
        TTF_CloseFont(_font);
}

void MovePanel::SetPosition(const fatpup::Position& pos)
{
    for (int c = White; c <= Black; ++c)
    {
        _movesString[c].clear();
        if (!pos.isWhiteTurn() && c == White)
            _movesString[c] += "...\n";
    }

    _position = pos;
    _positionUpdated = true;
}

void MovePanel::Move(fatpup::Move move)
{
    const auto c = (_position.isWhiteTurn() ? White : Black);
    _movesString[c] += _position.moveToString(move) + "\n";

    _position += move;
    _positionUpdated = true;
}

void MovePanel::UpdateTexture(SDL_Surface* surface, SDL_Texture** texture, SDL_Rect* textureRect)
{
    if (*texture)
        SDL_DestroyTexture(*texture), *texture = nullptr;

    if (surface)
    {
        *texture = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_FreeSurface(surface);
    }

    if (*texture)
    {
        int texW = 0, texH = 0;
        SDL_QueryTexture(*texture, NULL, NULL, &texW, &texH);
        textureRect->w = texW;
        textureRect->h = texH;
    }
}

void MovePanel::UpdateTextures()
{
    if (!_positionUpdated)
        return;

    _positionUpdated = false;

    _positionState = _position.getState();

    if (_font)
    {
        static const SDL_Color colors[] = {{ 255, 255, 255, 255 }, { 0, 0, 0, 255 }};

        if (_positionState == fatpup::Position::State::Checkmate)
        {
            const auto c = _position.isWhiteTurn() ? Black : White;
            const std::string winText(std::string(c ? "Black" : "White") + " won");
            SDL_Surface* surface = TTF_RenderText_Blended(_font, winText.c_str(), colors[c]);
            UpdateTexture(surface, &_winDrawIndicatorTexture, &_winDrawIndicatorTextureRect);
        }
        else if (_positionState == fatpup::Position::State::Stalemate)
        {
            const SDL_Color gray = { 128, 128, 128, 255 };
            SDL_Surface* surface = TTF_RenderText_Blended(_font, "Draw", gray);
            UpdateTexture(surface, &_winDrawIndicatorTexture, &_winDrawIndicatorTextureRect);
        }

        for (int c = White; c <= Black; ++c)
        {
            SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(_font, _movesString[c].c_str(), colors[c], _backgroundRect.w / 2);
            UpdateTexture(surface, &_movesTexture[c], &_movesTextureRect[c]);
        }
    }
}

void MovePanel::Render()
{
    static const uint8_t backgroundColor[] =
    {
        (DARK_SQUARE[0] + LIGHT_SQUARE[0]) / 2,
        (DARK_SQUARE[1] + LIGHT_SQUARE[1]) / 2,
        (DARK_SQUARE[2] + LIGHT_SQUARE[2]) / 2
    };
    SDL_SetRenderDrawColor(_renderer, backgroundColor[0], backgroundColor[1], backgroundColor[2], 255);
    SDL_RenderFillRect(_renderer, &_backgroundRect);

    UpdateTextures();

    for (int c = White; c <= Black; ++c)
    {
        if (_movesTexture[c])
            SDL_RenderCopy(_renderer, _movesTexture[c], NULL, &_movesTextureRect[c]);
    }

    if (_positionState != fatpup::Position::State::Checkmate && _positionState != fatpup::Position::State::Stalemate)
    {
        const auto c = _position.isWhiteTurn() ? White : Black;
        if (_turnIndicatorTexture[c])
            SDL_RenderCopy(_renderer, _turnIndicatorTexture[c], NULL, &_turnIndicatorTextureRect[c]);
    }

    if (_positionState == fatpup::Position::State::Checkmate || _positionState == fatpup::Position::State::Stalemate)
    {
        if (_winDrawIndicatorTexture)
            SDL_RenderCopy(_renderer, _winDrawIndicatorTexture, NULL, &_winDrawIndicatorTextureRect);
    }
}
