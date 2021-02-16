#ifndef ENGINE_H
#define ENGINE_H

#include "misc.h"
#include <vector>

using MoveList  = std::vector<Move>;
using Board     = std::vector<std::pair<Piece, Square>>;

struct Engine {

    void        reset();
    void        act(Move move);

    MoveList    legal_moves() const;
    Board       board() const;

    Color       turn = BOTH;
private:
    friend std::ostream& operator<<(std::ostream &os, const Engine &e);

    bool        legal(Move move) const;

    Bitboard    all() const                    { return pieces[WHITE] | pieces[BLACK] | kings; }
    Bitboard    captures() const;
    Bitboard    man_moves(size_t sq) const     { return ATTACKS[turn][sq] & ~all(); }
    Bitboard    king_moves(size_t sq) const    { return ATTACKS[BOTH][sq] & ~all(); }
    Bitboard    man_capture_moves(size_t sq) const;
    Bitboard    king_capture_moves(size_t sq) const;

    Bitboard    pieces[BOTH] = {};
    Bitboard    kings = 0;
};

#endif // ENGINE_H
