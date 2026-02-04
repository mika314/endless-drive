#pragma once

#include "obstacle.hpp"

class Tire : public Obstacle
{
public:
  Tire(BaseNode *parent, class Assets &);
  auto tick(float) -> void final;
  auto onHit(float now) -> void final;

private:
  glm::vec3 vel = {};
};
