#include <iostream>

#include "engine.h"

#include "minimax.h"

Engine* Engine::create(const std::string& engineName, const fatpup::Position& pos)
{
    if (engineName == "minimax")
        return new MinimaxEngine(pos);

    std::cerr << "Unknown solver (" << engineName << ")\n";

    return nullptr;
}
