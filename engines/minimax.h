#ifndef FATPUP_ENGINES_MINIMAX_H
#define FATPUP_ENGINES_MINIMAX_H

#include "engine.h"

class MinimaxEngine: public Engine
{
public:
    MinimaxEngine(const fatpup::Position& pos);

    void Start() override;
    void Stop() override {}

    fatpup::Move GetBestMove() override { return _bestMove; }
    void MoveDone(fatpup::Move move) override;

private:
    fatpup::Position _pos;
    fatpup::Move _bestMove;
};

#endif // FATPUP_ENGINES_MINIMAX_H
