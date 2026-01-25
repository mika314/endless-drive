#pragma once
#include <functional>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <string>

class Font;

class Label
{
public:
  std::string text;
  std::reference_wrapper<Font> font;
  glm::vec3 color = {.5f, .5f, .5f};
  float size = 42.f;

protected:
  auto uiPass(class Render &, glm::mat3) const -> void;
};
