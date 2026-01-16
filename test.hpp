#pragma once
#include "assets.hpp"
#include "mesh.hpp"
#include <bgfx/bgfx.h>
#include <sdlpp/sdlpp.hpp>

class Test
{
public:
  Test(sdl::Window &, int w, int h);
  ~Test();
  auto tick() -> void;

private:
  sdl::Window &win;
  int w;
  int h;
  Assets assets;
  Mesh &car;
  bgfx::UniformHandle transUniform;
  bgfx::UniformHandle viewPosUniform;
  glm::vec3 camPos = glm::vec3{-2.5f, 0.0f, 0.5f};
  float camYaw = 0.0f;
  float camPitch = 0.0f;
  bgfx::UniformHandle baseColorTex;
  bgfx::UniformHandle metalicTex;
  bgfx::UniformHandle roughnessTex;
  bgfx::UniformHandle setup;
  bgfx::UniformHandle baseColor;
  bgfx::UniformHandle metalic;
  bgfx::UniformHandle roughness;
  bgfx::ProgramHandle prog;
};
