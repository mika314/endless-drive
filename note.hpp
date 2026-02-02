#pragma once
#include <cstdint>
#include <initializer_list>
#include <ser/macro.hpp>

struct Note
{
  double n = 60;
  double dur = 1;
  double vel = -12;
  SER_PROPS(n, dur, vel);
  auto setDur(double) -> Note &;
  auto setVel(double) -> Note &;
  auto setDur(double) const -> Note;
  auto setVel(double) const -> Note;
};

auto operator+(Note, double) -> Note;
auto operator+(double, Note) -> Note;
auto operator+=(Note &, double) -> Note &;
auto operator-(Note, double) -> Note;
auto operator-=(Note &, double) -> Note &;

struct root
{
  root(const Note &av) : v(av) {}
  Note v;
};

struct dur
{
  double v;
  auto operator*(double other) const -> dur { return dur{v * other}; }
  auto operator+(double other) const -> dur { return dur{v + other}; }
  auto operator-(double other) const -> dur { return dur{v - other}; }
};

struct vel
{
  double v;
};

struct Rest
{
};

template <typename NoteType>
struct chord
{
  std::initializer_list<NoteType> v;
};

constexpr Note C0 = {0};
constexpr Note Cs0 = {1};
constexpr Note Db0 = {1};
constexpr Note D0 = {2};
constexpr Note Ds0 = {3};
constexpr Note Eb0 = {3};
constexpr Note E0 = {4};
constexpr Note F0 = {5};
constexpr Note Fs0 = {6};
constexpr Note Gb0 = {6};
constexpr Note G0 = {7};
constexpr Note Gs0 = {8};
constexpr Note Ab0 = {8};
constexpr Note A0 = {9};
constexpr Note As0 = {10};
constexpr Note Bb0 = {10};
constexpr Note B_0 = {11};

constexpr Note C1 = {12};
constexpr Note Cs1 = {13};
constexpr Note Db1 = {13};
constexpr Note D1 = {14};
constexpr Note Ds1 = {15};
constexpr Note Eb1 = {15};
constexpr Note E1 = {16};
constexpr Note F1 = {17};
constexpr Note Fs1 = {18};
constexpr Note Gb1 = {18};
constexpr Note G1 = {19};
constexpr Note Gs1 = {20};
constexpr Note Ab1 = {20};
constexpr Note A1 = {21};
constexpr Note As1 = {22};
constexpr Note Bb1 = {22};
constexpr Note B1 = {23};

constexpr Note C2 = {24};
constexpr Note Cs2 = {25};
constexpr Note Db2 = {25};
constexpr Note D2 = {26};
constexpr Note Ds2 = {27};
constexpr Note Eb2 = {27};
constexpr Note E2 = {28};
constexpr Note F2 = {29};
constexpr Note Fs2 = {30};
constexpr Note Gb2 = {30};
constexpr Note G2 = {31};
constexpr Note Gs2 = {32};
constexpr Note Ab2 = {32};
constexpr Note A2 = {33};
constexpr Note As2 = {34};
constexpr Note Bb2 = {34};
constexpr Note B2 = {35};

constexpr Note C3 = {36};
constexpr Note Cs3 = {37};
constexpr Note Db3 = {37};
constexpr Note D3 = {38};
constexpr Note Ds3 = {39};
constexpr Note Eb3 = {39};
constexpr Note E3 = {40};
constexpr Note F3 = {41};
constexpr Note Fs3 = {42};
constexpr Note Gb3 = {42};
constexpr Note G3 = {43};
constexpr Note Gs3 = {44};
constexpr Note Ab3 = {44};
constexpr Note A3 = {45};
constexpr Note As3 = {46};
constexpr Note Bb3 = {46};
constexpr Note B3 = {47};

constexpr Note C4 = {48};
constexpr Note Cs4 = {49};
constexpr Note Db4 = {49};
constexpr Note D4 = {50};
constexpr Note Ds4 = {51};
constexpr Note Eb4 = {51};
constexpr Note E4 = {52};
constexpr Note F4 = {53};
constexpr Note Fs4 = {54};
constexpr Note Gb4 = {54};
constexpr Note G4 = {55};
constexpr Note Gs4 = {56};
constexpr Note Ab4 = {56};
constexpr Note A4 = {57};
constexpr Note As4 = {58};
constexpr Note Bb4 = {58};
constexpr Note B4 = {59};

constexpr Note C5 = {60};
constexpr Note Cs5 = {61};
constexpr Note Db5 = {61};
constexpr Note D5 = {62};
constexpr Note Ds5 = {63};
constexpr Note Eb5 = {63};
constexpr Note E5 = {64};
constexpr Note F5 = {65};
constexpr Note Fs5 = {66};
constexpr Note Gb5 = {66};
constexpr Note G5 = {67};
constexpr Note Gs5 = {68};
constexpr Note Ab5 = {68};
constexpr Note A5 = {69};
constexpr Note As5 = {70};
constexpr Note Bb5 = {70};
constexpr Note B5 = {71};

constexpr Note C6 = {72};
constexpr Note Cs6 = {73};
constexpr Note Db6 = {73};
constexpr Note D6 = {74};
constexpr Note Ds6 = {75};
constexpr Note Eb6 = {75};
constexpr Note E6 = {76};
constexpr Note F6 = {77};
constexpr Note Fs6 = {78};
constexpr Note Gb6 = {78};
constexpr Note G6 = {79};
constexpr Note Gs6 = {80};
constexpr Note Ab6 = {80};
constexpr Note A6 = {81};
constexpr Note As6 = {82};
constexpr Note Bb6 = {82};
constexpr Note B6 = {83};

constexpr Note C7 = {84};
constexpr Note Cs7 = {85};
constexpr Note Db7 = {85};
constexpr Note D7 = {86};
constexpr Note Ds7 = {87};
constexpr Note Eb7 = {87};
constexpr Note E7 = {88};
constexpr Note F7 = {89};
constexpr Note Fs7 = {90};
constexpr Note Gb7 = {90};
constexpr Note G7 = {91};
constexpr Note Gs7 = {92};
constexpr Note Ab7 = {92};
constexpr Note A7 = {93};
constexpr Note As7 = {94};
constexpr Note Bb7 = {94};
constexpr Note B7 = {95};
constexpr Note C8 = {96};

using Interval = double;

constexpr Interval I = 0;
constexpr Interval ii = 1;
constexpr Interval II = 2;
constexpr Interval iii = 3;
constexpr Interval III = 4;
constexpr Interval IV = 5;
constexpr Interval V = 7;
constexpr Interval vi = 8;
constexpr Interval VI = 9;
constexpr Interval vii = 10;
constexpr Interval VII = 11;

constexpr Interval O = 12;

constexpr Interval VIII = O;
constexpr Interval ix = 1 + O;
constexpr Interval IX = 2 + O;
constexpr Interval x = 3 + O;
constexpr Interval X = 4 + O;
constexpr Interval XI = 5 + O;
constexpr Interval XII = 7 + O;
constexpr Interval xiii = 8 + O;
constexpr Interval XIII = 9 + O;
constexpr Interval xiv = 10 + O;
constexpr Interval XIV = 11 + O;

static auto Sixteenth = dur{.25};
static auto Eighth = dur{.5};
static auto Quarter = dur{1.};
static auto Halth = dur{2.};
static auto Whole = dur{4.};
static auto Bar = dur{4.};
static auto d32 = dur{.125};
static auto d64 = dur{.0625};
static auto d16 = dur{.25};
static auto d8 = dur{.5};
static auto d4 = dur{1.};
static auto d2 = dur{2.};
static auto d1 = dur{4.};
static auto rest = Rest{};

#define ChI(...)  \
  chord<Interval> \
  {               \
    {             \
      __VA_ARGS__ \
    }             \
  }
#define ChN(...)  \
  chord<Note>     \
  {               \
    {             \
      __VA_ARGS__ \
    }             \
  }
