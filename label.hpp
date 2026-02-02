#pragma once
#include <functional>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>

class Font;

class Label
{
public:
  std::string text;
  std::reference_wrapper<Font> font;
  glm::vec3 color = {.5f, .5f, .5f};
  float sz = 42.f;

  auto uiPass(class Render &, glm::mat4) const -> void;
};
