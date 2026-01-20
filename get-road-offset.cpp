#include "get-road-offset.hpp"
#include <cmath>

auto getRoadOffset(float y) -> float
{
  return 5.f * sinf(y * 0.05f) + 2.5f * sinf(y * 0.1);
}
