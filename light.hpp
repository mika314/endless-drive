#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Light
{
public:
  glm::vec3 color;
  auto geomPass(class Render &, glm::mat4) const -> void;
  auto lightPass(class Render &, glm::vec3 pos) const -> void;
};
