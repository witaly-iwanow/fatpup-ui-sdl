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
    }

    _backgroundRect.x = xOff;
    _backgroundRect.y = 0;
    _backgroundRect.w = width;
    _backgroundRect.h = height;

    _movesTextureRect[White].x = _backgroundRect.x + _backgroundRect.w / 32;
    _movesTextureRect[White].y = _backgroundRect.w / 8;
    _movesTextureRect[Black].x = _movesTextureRect[White].x + _backgroundRect.w / 2;
    _movesTextureRect[Black].y = _movesTextureRect[White].y;

    _position.setInitial();
}

MovePanel::~MovePanel()
{
    for (int c = White; c <= Black; ++c)
    {
        if (_movesTexture[c])
            SDL_DestroyTexture(_movesTexture[c]);
    }

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
    PositionUpdated();
}

void MovePanel::Move(fatpup::Move move)
{
    const auto c = (_position.isWhiteTurn() ? White : Black);
    _movesString[c] += _position.moveToString(move) + "\n";

    _position += move;
    PositionUpdated();
}

void MovePanel::PositionUpdated()
{
    _positionState = _position.getState();
/*
    if (_positionState == fatpup::Position::State::Checkmate)
    {
        _winDrawIndicator.setString(std::string(_position.isWhiteTurn() ? "Black" : "White") +  " won");
        _winDrawIndicator.setFillColor(_position.isWhiteTurn() ? sf::Color::Black : sf::Color::White);
    }
    else if (_positionState == fatpup::Position::State::Stalemate)
    {
        _winDrawIndicator.setString("Draw");
        _winDrawIndicator.setFillColor(sf::Color(128, 128, 128));
    }
*/

    if (_font)
    {
        SDL_Color colors[] = {{ 255, 255, 255, 255 }, { 0, 0, 0, 255 }};
        for (int c = White; c <= Black; ++c)
        {
            SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(_font, _movesString[c].c_str(), colors[c], _backgroundRect.w / 2);
            if (surface)
            {
                if (_movesTexture[c])
                    SDL_DestroyTexture(_movesTexture[c]), _movesTexture[c] = nullptr;

                _movesTexture[c] = SDL_CreateTextureFromSurface(_renderer, surface);
                SDL_FreeSurface(surface);

                if (_movesTexture[c])
                {
                    int texW = 0, texH = 0;
                    SDL_QueryTexture(_movesTexture[c], NULL, NULL, &texW, &texH);
                    _movesTextureRect[c].w = texW;
                    _movesTextureRect[c].h = texH;
                }
            }
        }
    }
}

void MovePanel::Render() const
{
    static const uint8_t backgroundColor[] =
    {
        (DARK_SQUARE[0] + LIGHT_SQUARE[0]) / 2,
        (DARK_SQUARE[1] + LIGHT_SQUARE[1]) / 2,
        (DARK_SQUARE[2] + LIGHT_SQUARE[2]) / 2
    };
    SDL_SetRenderDrawColor(_renderer, backgroundColor[0], backgroundColor[1], backgroundColor[2], 255);
    SDL_RenderFillRect(_renderer, &_backgroundRect);

    for (int c = White; c <= Black; ++c)
    {
        if (_movesTexture[c])
            SDL_RenderCopy(_renderer, _movesTexture[c], NULL, &_movesTextureRect[c]);
    }
/*
    window.draw(_background);
    if (_positionState != fatpup::Position::State::Checkmate && _positionState != fatpup::Position::State::Stalemate)
        window.draw(_turnIndicator[_position.isWhiteTurn() ? White : Black]);
    window.draw(_moves[White]);
    window.draw(_moves[Black]);

    if (_positionState == fatpup::Position::State::Checkmate || _positionState == fatpup::Position::State::Stalemate)
        window.draw(_winDrawIndicator);
*/
}
