#ifndef FATPUP_ENGINES_ENGINE_H
#define FATPUP_ENGINES_ENGINE_H

#include <string>

#include "fatpup/position.h"

class Engine
{
public:
    static Engine* create(const std::string& solver_name, const fatpup::Position& pos);

    virtual ~Engine() {}

    virtual void Start() = 0;
    virtual void Stop() = 0;

    virtual fatpup::Move GetBestMove() = 0;
    virtual void MoveDone(fatpup::Move move) = 0;
};

#endif // FATPUP_ENGINES_ENGINE_H
