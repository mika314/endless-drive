#include "obstacle.hpp"
#include <sdlpp/sdlpp.hpp>

auto Obstacle::onHit() -> void
{
  wasHit_ = true;
}

auto Obstacle::wasHit() const -> bool
{
  return wasHit_;
}
