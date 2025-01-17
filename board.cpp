#include <iostream>
#include <algorithm>
#include <chrono>

#include "colors.h"

#include "movepanel.h"
#include "board.h"

Board::Board(SDL_Renderer* renderer, int windowWidth, int windowHeight, bool playingWhite):
    _renderer(renderer),
    _playingWhite(playingWhite)
{
    auto smallestDimension = std::min(windowWidth, windowHeight) / fatpup::BOARD_SIZE;
    if (smallestDimension < 16)
        smallestDimension = 16;
    _squareSize = (float)smallestDimension;

    LoadPieceTexture(fatpup::White | fatpup::King, "resources/WhiteKing.png");
    LoadPieceTexture(fatpup::White | fatpup::Queen, "resources/WhiteQueen.png");
    LoadPieceTexture(fatpup::White | fatpup::Rook, "resources/WhiteRook.png");
    LoadPieceTexture(fatpup::White | fatpup::Bishop, "resources/WhiteBishop.png");
    LoadPieceTexture(fatpup::White | fatpup::Knight, "resources/WhiteKnight.png");
    LoadPieceTexture(fatpup::White | fatpup::Pawn, "resources/WhitePawn.png");

    LoadPieceTexture(fatpup::Black | fatpup::King, "resources/BlackKing.png");
    LoadPieceTexture(fatpup::Black | fatpup::Queen, "resources/BlackQueen.png");
    LoadPieceTexture(fatpup::Black | fatpup::Rook, "resources/BlackRook.png");
    LoadPieceTexture(fatpup::Black | fatpup::Bishop, "resources/BlackBishop.png");
    LoadPieceTexture(fatpup::Black | fatpup::Knight, "resources/BlackKnight.png");
    LoadPieceTexture(fatpup::Black | fatpup::Pawn, "resources/BlackPawn.png");

    _position.setEmpty();

    _lastMove.setEmpty();
}

Board::~Board()
{
    ShutdownEngineThread();

    for (auto t: _pieceTextures)
        SDL_DestroyTexture(t.second);
}

void Board::ShutdownEngineThread()
{
    if (_engineThread)
    {
        _shutdown = true;
        _engineCv.notify_one();
        _engineThread->join();
        delete _engineThread;
        _engineThread = nullptr;
    }
    _engine = nullptr;
}

void Board::SetEngine(fatpup::Engine* engine)
{
    ShutdownEngineThread();
    if (engine)
    {
        _shutdown = false;
        _engine = engine;
        _engineThread = new std::thread(&Board::EngineThreadFunc, this);
    }
}

void Board::LoadPieceTexture(unsigned char piece, const char* pieceTexturePath)
{
    SDL_Surface* surface = IMG_Load(pieceTexturePath);
    if (surface)
    {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
        if (texture)
            _pieceTextures[piece] = texture;
        else
            std::cerr << "SDL_CreateTextureFromSurface failed, error: " << SDL_GetError() << "\n";
    }
    else
        std::cerr << "IMG_Load(" << (pieceTexturePath ? "<null>" : pieceTexturePath) << ") failed, error: " << IMG_GetError() << "\n";

    SDL_FreeSurface(surface);
}

void Board::EngineThreadFunc()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(_engineMutex);
        _engineCv.wait(lock, [this]{ return !_lastMove.isEmpty() || _shutdown; });

        if (_shutdown)
            break;

        _engine->MoveDone(_lastMove);
        //_engine->Start();
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //_engine->Stop();

        const auto engineMove = _engine->GetBestMove();
        if (!engineMove.isEmpty())
            Move(engineMove);

        _lastMove.setEmpty();
    }
}

void Board::SetMovePanel(MovePanel* movePanel)
{
    _movePanel = movePanel;
    if (_movePanel)
        _movePanel->SetPosition(_position);
}

void Board::SetPosition(const fatpup::Position& pos)
{
    if (_movePanel)
        _movePanel->SetPosition(pos);

    _position = pos;
}

void Board::Move(fatpup::Move move)
{
    if (_movePanel)
        _movePanel->Move(move);

    _position += move;
    _lastMoveSquareIdx[0] = DisplayFatpupRow(move.fields.src_row) * fatpup::BOARD_SIZE + DisplayFatpupCol(move.fields.src_col);
    _lastMoveSquareIdx[1] = DisplayFatpupRow(move.fields.dst_row) * fatpup::BOARD_SIZE + DisplayFatpupCol(move.fields.dst_col);
}

void Board::ProcessUserMove(const int destSquareIdx)
{
    if (_selectedSquareIdx == -1)
    {
        // new square/piece selected
        const int row = DisplayFatpupRow(destSquareIdx / fatpup::BOARD_SIZE);
        const int col = DisplayFatpupCol(destSquareIdx % fatpup::BOARD_SIZE);
        const auto piece = _position.square(row, col).pieceWithColor();
        if (piece & fatpup::PieceMask)
        {
            // at least it's not empty, check the color - only allow to pick white pieces
            // on white turn, black ones on black turn
            if (_position.isWhiteTurn() == ((piece & fatpup::ColorMask) == fatpup::White))
                _selectedSquareIdx = destSquareIdx;
        }
    }
    else if (destSquareIdx == _selectedSquareIdx)
    {
        // same square/piece clicked again, remove selection
        _selectedSquareIdx = -1;
    }
    else
    {
        const int src_row = DisplayFatpupRow(_selectedSquareIdx / fatpup::BOARD_SIZE);
        const int src_col = DisplayFatpupCol(_selectedSquareIdx % fatpup::BOARD_SIZE);
        const int dst_row = DisplayFatpupRow(destSquareIdx / fatpup::BOARD_SIZE);
        const int dst_col = DisplayFatpupCol(destSquareIdx % fatpup::BOARD_SIZE);
        const auto moves = _position.possibleMoves(src_row, src_col, dst_row, dst_col);

        _selectedSquareIdx = -1;

        if (!moves.empty())
        {
            // the move is legit, do it. To do: "custom" promotions are not handled properly,
            // for now we just pick the first move which is promotion to queen
            _lastMove = moves[0];
            Move(moves[0]);
            RequestEngineMove(moves[0]);
        }
        else
        {
            // in case the player selects one of his pieces but then changes his mind and clicks another one
            ProcessUserMove(destSquareIdx);
        }
    }
}

void Board::RequestEngineMove(fatpup::Move move)
{
    _lastMove = move;
    _engineCv.notify_one();
}

void Board::OnClick(int posx, int posy)
{
    if (!_position.isWhiteTurn())
        return;

    int xIdx = (int)(posx / _squareSize);
    int yIdx = (int)(posy / _squareSize);

    if (xIdx >= 0 && xIdx < fatpup::BOARD_SIZE && yIdx >= 0 && yIdx < fatpup::BOARD_SIZE)
        ProcessUserMove(yIdx * fatpup::BOARD_SIZE + xIdx);
}

void Board::Render() const
{
    int idx = 0;
    for (int row = 0; row < fatpup::BOARD_SIZE; ++row)
    {
        for (int col = 0; col < fatpup::BOARD_SIZE; ++col)
        {
            SDL_Rect rect;
            rect.x = (int)(col * _squareSize);
            rect.y = (int)(row * _squareSize);
            rect.w = (int)_squareSize;
            rect.h = (int)_squareSize;

            const uint8_t* rgba = ((row ^ col) & 1) ? DARK_SQUARE : LIGHT_SQUARE;
            SDL_SetRenderDrawColor(_renderer, rgba[0], rgba[1], rgba[2], rgba[3]);
            SDL_RenderFillRect(_renderer, &rect);

            bool squareHasOverlay = false;
            if (_selectedSquareIdx != -1 && _selectedSquareIdx == (row * fatpup::BOARD_SIZE + col))
                rgba = SELECTED_SQUARE, squareHasOverlay = true;
            else if ((_lastMoveSquareIdx[0] != -1 && _lastMoveSquareIdx[0] == (row * fatpup::BOARD_SIZE + col)) ||
                     (_lastMoveSquareIdx[1] != -1 && _lastMoveSquareIdx[1] == (row * fatpup::BOARD_SIZE + col)))
                rgba = LAST_MOVE_SQUARE, squareHasOverlay = true;

            if (squareHasOverlay)
            {
                SDL_SetRenderDrawColor(_renderer, rgba[0], rgba[1], rgba[2], rgba[3]);
                SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(_renderer, &rect);
            }

            const auto piece = _position.square(DisplayFatpupRow(row), DisplayFatpupCol(col)).pieceWithColor();
            if (piece)
                SDL_RenderCopy(_renderer, _pieceTextures.at(piece), NULL, &rect);

            ++idx;
        }
    }
}

