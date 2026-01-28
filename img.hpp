#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

class Tex;

class Img
{
public:
  std::reference_wrapper<Tex> tex;
  glm::vec2 sz;
  glm::vec2 pivot;

protected:
  auto uiPass(class Render &, glm::mat4) const -> void;
};
