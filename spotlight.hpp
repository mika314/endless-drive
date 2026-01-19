#pragma one
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Spotlight
{
public:
  glm::vec3 color;
  float angle;
  auto geomPass(class Render &, glm::mat4) const -> void;
  auto lightPass(class Render &, glm::mat4) const -> void;
};
