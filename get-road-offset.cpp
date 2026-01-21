#include "get-road-offset.hpp"
#include <cmath>

auto getRoadOffset(float y) -> float
{
  return 5.f * sinf(y * 0.05f) + 2.5f * sinf(y * 0.1);
}

auto getRoadAngle(float y) -> float
{
  const auto dy = 5.f * 0.05f * cosf(y * 0.05f) + 2.5f * 0.1f * cosf(y * 0.1f);
  return atanf(dy);
}
