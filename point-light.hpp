#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class PointLight
{
public:
  glm::vec3 color;

protected:
  auto geomPass(class Render &, glm::mat4 trans) const -> void;
  auto lightPass(class Render &, glm::mat4 trans) const -> void;
};
