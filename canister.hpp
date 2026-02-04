#pragma once

#include "obstacle.hpp"
#include <glm/vec3.hpp>

class Canister final : public Obstacle
{
public:
  Canister(BaseNode *parent, class Assets &);
  auto tick(float) -> void final;
  auto onHit(float now) -> void final;

private:
  glm::vec3 vel = {};
};
