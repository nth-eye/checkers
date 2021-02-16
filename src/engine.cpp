#include "engine.h"
#include <cstddef>
#include <ostream>

void Engine::reset()
{
    pieces[WHITE] = 0x00000000'0055AA55;
    pieces[BLACK] = 0xAA55AA00'00000000;
    kings = 0x00000000'00000000;

    turn = WHITE;
}

void Engine::act(Move move)
{
    const auto f_bb = bitboard(move.from);
    const auto t_bb = bitboard(move.to);

    pieces[turn]    |= t_bb;
    if (kings & f_bb || move.type & PROMOTION)
        kings       |= t_bb;

    pieces[turn]    &= ~f_bb;
    kings           &= ~f_bb;

    if (move.type & CAPTURE) {
        switch (int(move.to) - int(move.from)) {
            case NORTH_EAST * 2:
                pieces[~turn]   &= ~(f_bb << NORTH_EAST);
                kings           &= ~(f_bb << NORTH_EAST);
                break;
            case NORTH_WEST * 2:
                pieces[~turn]   &= ~(f_bb << NORTH_WEST);
                kings           &= ~(f_bb << NORTH_WEST);
                break;
            case SOUTH_EAST * 2:
                pieces[~turn]   &= ~(t_bb << NORTH_WEST);
                kings           &= ~(t_bb << NORTH_WEST);
                break;
            case SOUTH_WEST * 2:
                pieces[~turn]   &= ~(t_bb << NORTH_EAST);
                kings           &= ~(t_bb << NORTH_EAST);
                break;
        }
        // If another move available with same piece
        if (man_capture_moves(move.to) || king_capture_moves(move.to))
            return;
    }
    turn = ~turn;
}

bool Engine::legal(Move move) const
{
    return (kings & bitboard(move.from) ? 
           (king_moves(move.from)   | king_capture_moves(move.from)) : 
           (man_moves(move.from)    | man_capture_moves(move.from)) )
           & bitboard(move.to);
}

Bitboard Engine::captures() const
{
    const auto non = ~all();
    const auto opp = pieces[~turn];
    const auto k = pieces[turn] & kings;

    auto captures = (shift(shift(k, NORTH_EAST) & opp, NORTH_EAST) & non) |
                    (shift(shift(k, NORTH_WEST) & opp, NORTH_WEST) & non) |
                    (shift(shift(k, SOUTH_EAST) & opp, SOUTH_EAST) & non) |
                    (shift(shift(k, SOUTH_WEST) & opp, SOUTH_WEST) & non);

    if (turn == WHITE) {
        captures |= (shift(shift(pieces[WHITE], NORTH_EAST) & opp, NORTH_EAST) & non) |
                    (shift(shift(pieces[WHITE], NORTH_WEST) & opp, NORTH_WEST) & non);
    } else {
        captures |= (shift(shift(pieces[BLACK], SOUTH_EAST) & opp, SOUTH_EAST) & non) |
                    (shift(shift(pieces[BLACK], SOUTH_WEST) & opp, SOUTH_WEST) & non);
    }
    return captures;
}

Bitboard Engine::man_capture_moves(size_t sq) const
{
    const auto non = ~all();

    if (turn == WHITE) {
        return (shift(R_ATTACKS[WHITE][sq] & pieces[BLACK], NORTH_EAST) & non) |
               (shift(L_ATTACKS[WHITE][sq] & pieces[BLACK], NORTH_WEST) & non);
    } else {
        return (shift(R_ATTACKS[BLACK][sq] & pieces[WHITE], SOUTH_EAST) & non) |
               (shift(L_ATTACKS[BLACK][sq] & pieces[WHITE], SOUTH_WEST) & non);
    }
}

Bitboard Engine::king_capture_moves(size_t sq) const
{
    const auto non = ~all();
    const auto opp = pieces[~turn];

    return (shift(R_ATTACKS[WHITE][sq] & opp, NORTH_EAST) & non) |
           (shift(L_ATTACKS[WHITE][sq] & opp, NORTH_WEST) & non) |
           (shift(R_ATTACKS[BLACK][sq] & opp, SOUTH_EAST) & non) |
           (shift(L_ATTACKS[BLACK][sq] & opp, SOUTH_WEST) & non);
}

MoveList Engine::legal_moves() const
{
    MoveList list;

    if (captures()) {

        for (const auto from : BitIterator(pieces[turn] & kings))
             for (const auto to : BitIterator(king_capture_moves(from)))
                 list.emplace_back(Square(from), Square(to), CAPTURE);

        for (const auto from : BitIterator(pieces[turn] & ~kings))
            for (const auto to : BitIterator(man_capture_moves(from)))
                list.emplace_back(Square(from), Square(to), bitboard(to) & OPPOSITE_RANK[turn] ? PROMOTION | CAPTURE : CAPTURE);

        return list;
    }

    for (const auto from : BitIterator(pieces[turn] & kings))
         for (const auto to : BitIterator(king_moves(from)))
             list.emplace_back(Square(from), Square(to), QUIET);

    for (const auto from : BitIterator(pieces[turn] & ~kings))
        for (const auto to : BitIterator(man_moves(from)))
            list.emplace_back(Square(from), Square(to), bitboard(to) & OPPOSITE_RANK[turn] ? PROMOTION : QUIET);

    return list;
}

Board Engine::board() const
{
    Board board;

    for (const auto sq : BitIterator(pieces[WHITE] & ~kings))
        board.push_back( {{ MAN, WHITE }, sq} );

    for (const auto sq : BitIterator(pieces[WHITE] & kings))
        board.push_back( {{ KING, WHITE }, sq} );

    for (const auto sq : BitIterator(pieces[BLACK] & ~kings))
        board.push_back( {{ MAN, BLACK }, sq} );

    for (const auto sq : BitIterator(pieces[BLACK] & kings))
        board.push_back( {{ KING, BLACK }, sq} );

    return board;
}


std::ostream& operator<<(std::ostream &os, const Engine &e)
{
    constexpr auto SIDE_STR = "wb-";
    constexpr auto RANK_STR = "12345678";
    constexpr auto FILE_STR = "abcdefgh";

    os << "\nGAME BOARD:\n\n";

    for (int r = 7; r >= 0; --r) {
        os << RANK_STR[r] << "   ";
        for (int f = 0; f <= 7; ++f) {

            Square sq = square(r, f);
            char c = '.';

            if (get(e.pieces[WHITE], sq))
                c = 'w';
            else if (get(e.pieces[BLACK], sq))
                c = 'b';

            if (get(e.kings, sq))
                c = toupper(c);

            os << c << ' ';
        }
        os << '\n';
    }
    os << "\n   ";
    for (int f = 0; f <= 7; ++f)
        os << ' ' << FILE_STR[f];
    os	<< "\n\nside:\t" << SIDE_STR[e.turn];

    return os;
}
