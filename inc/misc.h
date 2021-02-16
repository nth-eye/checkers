#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <array>
#include <string>
#if __cplusplus > 201703L
#include <bit>
#endif

constexpr size_t SQ_SIZE = 60; // For graphical use
constexpr size_t SQ_NUM = 64;

using Square = uint8_t;
using Bitboard = uint64_t;

enum Type   { DEAD, MAN, KING };
enum Color  { WHITE, BLACK, BOTH };
enum Status { GOING, WIN, DRAW, STOPPED };
enum Direction {
    NORTH =  8,
    EAST  =  1,
    SOUTH = -NORTH,
    WEST  = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
};
enum MoveType : uint8_t {
    QUIET = 0,
    CAPTURE = 1,
    PROMOTION = 2,
};

struct Move {
    Square from;
    Square to;
    uint8_t type = QUIET;
#if __cplusplus > 201703L
    bool operator<=>(const Move &rhs) const = default;
#else
    Move() = default;
    Move(Square f, Square t, uint8_t type_) : from(f), to(t), type(type_) {}
#endif
};

struct Piece {
    Type type = DEAD;
    Color color = BOTH;
};

constexpr Bitboard LIGHT_SQUARES    = 0x55AA55AA55AA55AAULL;
constexpr Bitboard DARK_SQUARES     = 0xAA55AA55AA55AA55ULL;
constexpr Bitboard BORDER_SQUARES   = 0xFF818181818181FFULL;

constexpr Bitboard FILE_A_BB = 0x0101010101010101ULL;
constexpr Bitboard FILE_B_BB = FILE_A_BB << 1;
constexpr Bitboard FILE_C_BB = FILE_A_BB << 2;
constexpr Bitboard FILE_D_BB = FILE_A_BB << 3;
constexpr Bitboard FILE_E_BB = FILE_A_BB << 4;
constexpr Bitboard FILE_F_BB = FILE_A_BB << 5;
constexpr Bitboard FILE_G_BB = FILE_A_BB << 6;
constexpr Bitboard FILE_H_BB = FILE_A_BB << 7;

constexpr Bitboard RANK_1_BB = 0xFFULL;
constexpr Bitboard RANK_2_BB = RANK_1_BB << (8 * 1);
constexpr Bitboard RANK_3_BB = RANK_1_BB << (8 * 2);
constexpr Bitboard RANK_4_BB = RANK_1_BB << (8 * 3);
constexpr Bitboard RANK_5_BB = RANK_1_BB << (8 * 4);
constexpr Bitboard RANK_6_BB = RANK_1_BB << (8 * 5);
constexpr Bitboard RANK_7_BB = RANK_1_BB << (8 * 6);
constexpr Bitboard RANK_8_BB = RANK_1_BB << (8 * 7);

constexpr Bitboard OPPOSITE_RANK[] = { RANK_8_BB, RANK_1_BB };

constexpr auto generate_sq_bb()
{
    std::array<Bitboard, SQ_NUM> sq_bb = {};
    for (Square sq = 0; sq < SQ_NUM; ++sq)
        sq_bb[sq] = Bitboard(1ULL << sq);
    return sq_bb;
}
constexpr auto SQ_BB = generate_sq_bb();

constexpr Square square(const int r, const int f)   { return (r << 3) + f; }
constexpr bool valid(const Square sq)               { return sq < 64; }
constexpr Color operator~(Color c)                  { return Color(c ^ BLACK); }
constexpr Bitboard bitboard(Square sq)              { return SQ_BB[sq]; }
constexpr void set(Bitboard &bb, Square sq)         { bb |= bitboard(sq); }
constexpr void clr(Bitboard &bb, Square sq)         { bb &= ~bitboard(sq); }
constexpr bool get(Bitboard bb, Square sq)          { return bb & bitboard(sq); }

#if __cplusplus > 201703L
constexpr int count(Bitboard bb)                    { return std::popcount(bb); }
constexpr int lsb(Bitboard bb)                      { return std::countr_zero(bb);  }
#else
constexpr int count(Bitboard bb)                    { int cnt = 0; while (bb) { cnt++; bb &= bb - 1; } return cnt; }
constexpr int lsb(Bitboard bb)                      { return count((bb & -bb) - 1);  }
#endif

constexpr Bitboard shift(Bitboard b, Direction d)
{
    switch (d) {
        case NORTH:      return  b << 8;
        case SOUTH:      return  b >> 8;
        case EAST:       return (b & ~FILE_H_BB) << 1;
        case WEST:       return (b & ~FILE_A_BB) >> 1;
        case NORTH_EAST: return (b & ~FILE_H_BB) << 9;
        case NORTH_WEST: return (b & ~FILE_A_BB) << 7;
        case SOUTH_EAST: return (b & ~FILE_H_BB) >> 7;
        case SOUTH_WEST: return (b & ~FILE_A_BB) >> 9;
        default: return 0;
    }
}

#if __cplusplus > 201703L
constexpr Bitboard shift(Bitboard b, auto dirs)
#else
template<class T>
constexpr Bitboard shift(Bitboard b, T dirs)
#endif
{
    Bitboard bb = 0;
    for (const auto d : dirs)
        bb |= shift(b, d);
    return bb;
}

#if __cplusplus > 201703L
constexpr auto generate_attacks(auto dir)
#else
template<class T>
constexpr auto generate_attacks(T dir)
#endif
{
    using namespace std;

    array<array<Bitboard, 64>, 3> attacks = {};

    for (Square sq = 0; sq < SQ_NUM; ++sq) {

        const auto bb = bitboard(sq);

        attacks[WHITE][sq] = shift(bb, dir[WHITE]);
        attacks[BLACK][sq] = shift(bb, dir[BLACK]);
        attacks[BOTH][sq] = attacks[WHITE][sq] | attacks[BLACK][sq];
    }
    return attacks;
}
constexpr auto R_ATTACKS = generate_attacks(std::array{NORTH_EAST, SOUTH_EAST});
constexpr auto L_ATTACKS = generate_attacks(std::array{NORTH_WEST, SOUTH_WEST});
constexpr auto ATTACKS = generate_attacks(std::array{std::array{NORTH_EAST, NORTH_WEST}, std::array{SOUTH_EAST, SOUTH_WEST}});

inline std::string to_str(Bitboard bb)
{
    std::string str;
    for (int rank = 7; rank >= 0; --rank) {
        str += '|';
        for (int file = 0; file <= 7; ++file) {
            str += get(bb, square(rank, file)) ? 'X' : '-';
            str += '|';
        }
        str += '\n';
    }
    return str;
}

template <typename T>
struct BitIterator {

    constexpr BitIterator(T value_) : value(value_) {}

    constexpr bool  operator!=(const BitIterator &other) { return value != other.value; }
    constexpr void  operator++()                         { value &= (value - 1); }
    constexpr T     operator*() const                    { return lsb(value); }

    constexpr BitIterator begin() { return value; }
    constexpr BitIterator end()   { return 0; }
private:
    T value;
};

#endif // TYPES_H
