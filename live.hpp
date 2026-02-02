#pragma once

#include "obstacle.hpp"
#include <glm/vec3.hpp>

class Live final : public Obstacle
{
public:
  Live(BaseNode *parent, class Assets &);
  auto tick(float) -> void final;
  auto onHit() -> void final;

private:
  glm::vec3 vel = {};
};
