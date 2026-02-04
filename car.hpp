#pragma once

#include "mesh-node.hpp"

class Car final : public MeshNode
{
public:
  Car(BaseNode *parent, class Assets &);
  auto tick(float) -> void final;
  static auto desiredY(float now) -> float;

  int currentLane = 0;
  float now = 0.0f;
};
