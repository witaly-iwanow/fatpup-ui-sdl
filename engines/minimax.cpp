#include <iostream>
#include <limits>

#include "minimax.h"

class MinimaxPosition:
    public fatpup::Position
{
public:
    MinimaxPosition(const fatpup::Position& prevPos, fatpup::Move move):
        Position(prevPos)
    {
        moveDone(move);
    }

    int EvaluateMaterial() const
    {
        int eval = 0;
        for (int s_idx = 0; s_idx < fatpup::BOARD_SIZE * fatpup::BOARD_SIZE; ++s_idx)
            eval += m_board[s_idx].value();

        return eval;
    }
};

MinimaxEngine::MinimaxEngine(const fatpup::Position& pos):
    _pos(pos)
{
}

void MinimaxEngine::MoveDone(fatpup::Move move)
{
    _pos += move;
    FindBestMove();
}

void MinimaxEngine::FindBestMove()
{
    const auto moves = _pos.possibleMoves();
    fatpup::Move bestMove;
    int bestMoveEval = std::numeric_limits<int>::lowest();
    const bool reverseEval = !_pos.isWhiteTurn();

    for (auto move: moves)
    {
        const MinimaxPosition pos(_pos, move);
        const auto state = pos.getState();
        if (state == fatpup::Position::State::Checkmate)
        {
            bestMove = move;
            break;
        }

        int eval = pos.EvaluateMaterial() * 128;
        if (state != fatpup::Position::State::Stalemate)
        {
            if (reverseEval)
                eval = -eval;

            // we like checks
            if (state == fatpup::Position::State::Check)
                eval += 64;

            // and mobility
            eval -= pos.possibleMoves().size();
            if (state == fatpup::Position::State::Normal)
            {
                // it doesn't make sense to toggle a check position for example,
                // do this for normal states only
                fatpup::Position mobilityTestPos(pos);
                mobilityTestPos.toggleTurn();
                eval += mobilityTestPos.possibleMoves().size();
            }
        }
        else
            eval = 0;

        if (eval > bestMoveEval)
        {
            bestMoveEval = eval;
            bestMove = move;
        }
    }

    _bestMove = bestMove;
    if (!bestMove.isEmpty())
        _pos += bestMove;
}
