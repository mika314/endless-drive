#pragma once

#include "obstacle.hpp"
#include <glm/vec3.hpp>

class Multiplier final : public Obstacle
{
public:
  Multiplier(BaseNode *parent, class Assets &);
  auto tick(float) -> void final;
  auto onHit(float now) -> void final;

private:
  glm::vec3 vel = {};
};
