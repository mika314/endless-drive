#include "note.hpp"

auto operator+(Note x, double y) -> Note
{
  auto r = x;
  r.n += y;
  return r;
}

auto operator+(double y, Note x) -> Note
{
  auto r = x;
  r.n += y;
  return r;
}

auto operator+=(Note &x, double y) -> Note &
{
  x.n += y;
  return x;
}

auto operator-(Note x, double y) -> Note
{
  auto r = x;
  r.n -= y;
  return r;
}

auto operator-=(Note &x, double y) -> Note &
{
  x.n -= y;
  return x;
}

auto Note::setDur(double v) -> Note &
{
  dur = v;
  return *this;
}

auto Note::setVel(double v) -> Note &
{
  vel = v;
  return *this;
}

auto Note::setDur(double v) const -> Note
{
  auto copy = *this;
  copy.dur = v;
  return copy;
}

auto Note::setVel(double v) const -> Note
{
  auto copy = *this;
  copy.vel = v;
  return copy;
}
