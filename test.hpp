#pragma once
#include "assets.hpp"
#include "mesh.hpp"
#include "uni.hpp"
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
  float camYaw = 0.0f;
  float camPitch = 0.0f;
  Uni<glm::mat4x4> u_trans = "trans";
  Uni<glm::vec4> u_camPos = {"camPos", glm::vec4{-2.5f, 0.0f, .5f, 1.f}};
  Uni<glm::mat4x4> u_mtx = "mtx";
  Uni<Tex> u_baseColorTex = {"baseColorTex", 0};
  Uni<Tex> u_metallicTex = {"metallicTex", 1};
  Uni<Tex> u_roughnessTex = {"roughnessTex", 2};
  Uni<glm::vec4> u_settings = "settings";
  Uni<glm::vec4> u_baseColor = "baseColor";
  Uni<glm::vec4> u_metallic = "metallic";
  Uni<glm::vec4> u_roughness = "roughness";
  Uni<glm::vec4> u_lightPos = {"lightPos", glm::vec4{-1.25f, 1.f, 2.f, 0.f}};
  Uni<glm::vec4> u_lightColor = {"lightColor", glm::vec4{1.f}};

  class Deferrd
  {
  public:
    Deferrd(int w, int h);
    Deferrd(const Deferrd &) = delete;
    ~Deferrd();
    auto geom() -> void;
    auto light() -> void;
    auto combine() -> void;

  private:
    bgfx::TextureHandle t_baseColor;
    bgfx::TextureHandle t_metallicRoughness;
    bgfx::TextureHandle t_normals;
    bgfx::TextureHandle t_depth;
    std::array<bgfx::Attachment, 4> gBufferAt;
    bgfx::FrameBufferHandle gBuffer;
    bgfx::TextureHandle t_lightBuffer;
    bgfx::FrameBufferHandle lightBuffer;
    Uni<Tex> u_normals = {"normals", 0};
    Uni<Tex> u_metallicRoughness = {"metallicRoughness", 1};
    Uni<Tex> u_depth = {"depth", 2};
    Uni<Tex> u_baseColor = {"baseColor", 0};
    Uni<Tex> u_lightBuffer = {"lightBuffer", 1};
    const bgfx::Caps *caps;
  } deferrd;

  bgfx::ProgramHandle geom;
  bgfx::ProgramHandle light;
  bgfx::ProgramHandle combine;
};
