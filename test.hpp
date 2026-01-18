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
  glm::vec3 camPos = glm::vec3{-2.5f, 0.0f, 0.5f};
  float camYaw = 0.0f;
  float camPitch = 0.0f;
  bgfx::UniformHandle u_trans;
  bgfx::UniformHandle u_camPos;
  bgfx::UniformHandle u_mtx;
  bgfx::UniformHandle u_baseColorTex;
  bgfx::UniformHandle u_metallicTex;
  bgfx::UniformHandle u_roughnessTex;
  bgfx::UniformHandle u_settings;
  bgfx::UniformHandle u_baseColor;
  bgfx::UniformHandle u_metallic;
  bgfx::UniformHandle u_roughness;

  class Deferrd
  {
  public:
    Deferrd(int w, int h);
    Deferrd(const Deferrd &) = delete;
    ~Deferrd();

    // private: TODO-Mika make it private
    bgfx::TextureHandle t_baseColor;
    bgfx::TextureHandle t_metallicRoughness;
    bgfx::TextureHandle t_normals;
    bgfx::TextureHandle t_depth;
    std::array<bgfx::Attachment, 4> gBufferAt;
    bgfx::FrameBufferHandle gBuffer;
    bgfx::TextureHandle t_lightBuffer;
    bgfx::FrameBufferHandle lightBuffer;
    bgfx::UniformHandle u_baseColor;
    bgfx::UniformHandle u_metallicRoughness;
    bgfx::UniformHandle u_normals;
    bgfx::UniformHandle u_depth;
    bgfx::UniformHandle u_lightBuffer;
  } deferrd;

  bgfx::ProgramHandle geom;
  bgfx::ProgramHandle light;
  bgfx::ProgramHandle combine;
  const bgfx::Caps *caps;
};
