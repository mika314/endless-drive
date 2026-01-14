#pragma once
#include <bgfx/bgfx.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Vert
{
public:
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec2 uv;
  static bgfx::VertexLayout msLayout;
};
