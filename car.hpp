#pragma once

#include "mesh.hpp"
#include "node.hpp"

class Car : public VisualNodeRef<Mesh>
{
public:
  Car(BaseNode *parent, class Assets &);
  auto tick(float) -> void final;
};
