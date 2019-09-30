#include <iostream>

#include "board.h"
#include "colors.h"

extern "C" void umaxSetInitial();
extern "C" void umaxMakeMove(const char* m);
extern "C" void umaxGetMove(char* m);

Board::Board(SDL_Renderer* renderer, int windowWidth, int windowHeight, bool playingWhite):
    _renderer(renderer),
    _playingWhite(playingWhite)
{
    auto smallestDimension = std::min(windowWidth, windowHeight) / fatpup::BOARD_SIZE;
    if (smallestDimension < 16)
        smallestDimension = 16;
    _squareSize = smallestDimension;

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
    _engineThread = new std::thread(&Board::EngineThreadFunc, this);
}

Board::~Board()
{
    for (auto t: _pieceTextures)
        SDL_DestroyTexture(t.second);

    _shutdown = true;
    _engineCv.notify_one();
    _engineThread->join();
    delete _engineThread;
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

        char move[4];
        move[0] = 'a' + _lastMove.fields.src_col;
        move[1] = '1' + _lastMove.fields.src_row;
        move[2] = 'a' + _lastMove.fields.dst_col;
        move[3] = '1' + _lastMove.fields.dst_row;
        umaxMakeMove(move);
        umaxGetMove(move);

        const auto moves = _position.possibleMoves(move[1] - '1', move[0] - 'a', move[3] - '1', move[2] - 'a');
        if (!moves.empty())
            Move(moves[0]);

        _lastMove.setEmpty();
    }
}

void Board::SetPosition(const fatpup::Position& pos)
{
    _position = pos;
    umaxSetInitial();
}

void Board::Move(fatpup::Move move)
{
    _position += move;
}

void Board::ProcessUserMove(const int destSquareIdx)
{
    if (_selectedSquareIdx == -1)
    {
        // new square/piece selected
        const int row = DisplayRowToFatpup(destSquareIdx / fatpup::BOARD_SIZE);
        const int col = DisplayColToFatpup(destSquareIdx % fatpup::BOARD_SIZE);
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
        const int src_row = DisplayRowToFatpup(_selectedSquareIdx / fatpup::BOARD_SIZE);
        const int src_col = DisplayColToFatpup(_selectedSquareIdx % fatpup::BOARD_SIZE);
        const int dst_row = DisplayRowToFatpup(destSquareIdx / fatpup::BOARD_SIZE);
        const int dst_col = DisplayColToFatpup(destSquareIdx % fatpup::BOARD_SIZE);
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
    {
        ProcessUserMove(yIdx * fatpup::BOARD_SIZE + xIdx);

        //if (_selectedSquareIdx != -1)
        //    _selectedSquare.setPosition(xIdx * _squareSize + SELECTED_BORDER_WIDTH, yIdx * _squareSize + SELECTED_BORDER_WIDTH);
    }
}

void Board::Render() const
{
    int idx = 0;
    for (int row = 0; row < fatpup::BOARD_SIZE; ++row)
    {
        for (int col = 0; col < fatpup::BOARD_SIZE; ++col)
        {
            SDL_Rect rect;
            rect.x = col * _squareSize;
            rect.y = row * _squareSize;
            rect.w = _squareSize;
            rect.h = _squareSize;

            if ((row ^ col) & 1)
                SDL_SetRenderDrawColor(_renderer, DARK_SQUARE_R, DARK_SQUARE_G, DARK_SQUARE_B, 0xff);
            else
                SDL_SetRenderDrawColor(_renderer, LIGHT_SQUARE_R, LIGHT_SQUARE_G, LIGHT_SQUARE_B, 0xff);

            SDL_RenderFillRect(_renderer, &rect);

            //if (idx == _selectedSquareIdx)
            //    window.draw(_selectedSquare);

            const auto piece = _position.square(DisplayRowToFatpup(row), DisplayColToFatpup(col)).pieceWithColor();
            if (piece)
                SDL_RenderCopy(_renderer, _pieceTextures.at(piece), NULL, &rect);

            ++idx;
        }
    }
}
