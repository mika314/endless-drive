#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Light
{
public:
  glm::vec4 color;
  auto geomPass(class Render &) const -> void;
  auto lightPass(class Render &, glm::vec3 pos) const -> void;
};
