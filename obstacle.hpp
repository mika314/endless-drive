#pragma once
#include "mesh-node.hpp"

class Obstacle : public MeshNode
{
public:
  int x;
  float y;
  using MeshNode::MeshNode;
  virtual ~Obstacle() = default;
  virtual auto onHit() -> void = 0;
};
