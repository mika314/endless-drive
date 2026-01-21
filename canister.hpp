#pragma once

#include "mesh-node.hpp"

class Canister final : public MeshNode
{
public:
  Canister(BaseNode *parent, class Assets &);
  auto tick(float) -> void final;
};
