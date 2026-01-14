#include "vert.hpp"

bgfx::VertexLayout Vert::msLayout = []() {
  auto r = bgfx::VertexLayout{};
  r.begin()
    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
    .end();
  return r;
}();
