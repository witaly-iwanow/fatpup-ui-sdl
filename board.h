#ifndef FATPUP_UI_BOARD_H
#define FATPUP_UI_BOARD_H

#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "SDL.h"
#include "SDL_image.h"

#include "fatpup/position.h"

#include "engines/engine.h"

class MovePanel;

class Board
{
public:
    explicit Board(SDL_Renderer* renderer, int windowWidth, int windowHeight, bool playingWhite = true);
    virtual ~Board();

    void SetEngine(Engine* engine);
    void SetMovePanel(MovePanel* movePanel);
    void SetPosition(const fatpup::Position& pos);
    void Move(fatpup::Move move);

    void OnClick(int posx, int posy);

    void Render() const;

private:
    void LoadPieceTexture(unsigned char piece, const char* pieceTexturePath);
    void ProcessUserMove(const int destSquareIdx);

    void EngineThreadFunc();
    void RequestEngineMove(fatpup::Move move);
    void ShutdownEngineThread();

    // these toggle row/col from display to fatpup and back
    inline int DisplayFatpupRow(int row) const { return (_playingWhite ? (fatpup::BOARD_SIZE - 1 - row) : row); }
    inline int DisplayFatpupCol(int col) const { return (_playingWhite ? col : (fatpup::BOARD_SIZE - 1 - col)); }

    SDL_Renderer* _renderer = nullptr;
    int _selectedSquareIdx = -1;
    int _lastMoveSquareIdx[2] = { -1, -1 };

    std::map<unsigned char, SDL_Texture*> _pieceTextures;
    fatpup::Position _position;
    MovePanel* _movePanel = nullptr;

    float _squareSize;
    bool _playingWhite;

    fatpup::Move _lastMove;

    Engine* _engine = nullptr;
    std::thread* _engineThread = nullptr;
    std::mutex _engineMutex;
    std::condition_variable _engineCv;
    std::atomic<bool> _shutdown{false};
};

#endif // #define FATPUP_UI_BOARD_H
