#pragma once
#include "mesh-node.hpp"

class Obstacle : public MeshNode
{
public:
  int x;
  float y;
  using MeshNode::MeshNode;
  virtual ~Obstacle() = default;
  virtual auto onHit(float now) -> void;
  auto wasHit() const -> bool;

private:
  bool wasHit_ = false;
};
