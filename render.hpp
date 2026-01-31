#pragma once
#include "cube-atlas.hpp"
#include "font-manager.hpp"
#include "scene.hpp"
#include "text-buffer-manager.hpp"
#include "uni.hpp"
#include <bgfx/bgfx.h>
#include <sdlpp/sdlpp.hpp>

class Render
{
public:
  Render(sdl::Window &, int w, int h);
  ~Render();
  auto render(const Scene &) -> void;
  auto setCamPos(glm::vec3) -> void;
  auto setCamRot(glm::vec3) -> void;
  auto setMaterialAndRender(const class Material *) -> void;
  auto setPointLightAndRender(glm::vec3 pos, glm::vec3 color) -> void;
  auto setSpotlightAndRender(glm::mat4 trans, glm::vec3 color, float angle) -> void;

  struct TextIn
  {
    const std::string &text;
    const Font &font;
    float sz;
    glm::vec3 color;
    glm::mat4 trans;
  };
  auto operator()(const TextIn &) -> void;

  struct ImgIn
  {
    const Tex &tex;
  };
  auto operator()(const ImgIn &) -> void;

private:
  sdl::Window &win;
  int w;
  int h;
  Atlas atlas;
  TextBufferManager textBufferManager;
  TextBufferHandle textBuffer;
  glm::vec3 camPos = {0.f, -5.f, 1.8f};
  glm::vec3 camRot = {-0.3f, 0.0f, 0.0f};
  Uni<glm::vec4> u_camPos = {"camPos"};
  Uni<glm::mat4x4> u_mtx = "mtx";
  Uni<glm::mat4x4> u_projViewCombine = "projViewCombine";
  Uni<Tex> u_baseColorTex = {"baseColorTex", 0};
  Uni<Tex> u_metallicTex = {"metallicTex", 1};
  Uni<Tex> u_roughnessTex = {"roughnessTex", 2};
  Uni<Tex> u_emissionTex = {"emissionTex", 3};
  Uni<glm::vec4> u_settings = "settings";
  Uni<glm::vec4> u_baseColor = "baseColor";
  Uni<glm::vec4> u_metallic = "metallic";
  Uni<glm::vec4> u_roughness = "roughness";
  Uni<glm::vec4> u_emission = "emission";
  Uni<glm::vec4> u_lightPos = {"lightPos", glm::vec4{-1.25f, 1.f, 2.f, 0.f}};
  Uni<glm::vec4> u_lightColor = {"lightColor", glm::vec4{1.f}};
  Uni<glm::mat4> u_lightTrans = "lightTrans";
  Uni<glm::vec4> u_lightAngle = {"lightAngle", glm::vec4{.4f}};
  Uni<Tex> s_texColor = {"s_texColor", 0};
  Uni<Tex> u_imgTex = {"imgTex", 0};

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
    bgfx::TextureHandle t_emission;
    bgfx::TextureHandle t_depth;
    std::array<bgfx::Attachment, 5> gBufferAt;
    bgfx::FrameBufferHandle gBuffer;
    bgfx::TextureHandle t_lightBuffer;
    bgfx::FrameBufferHandle lightBuffer;
    Uni<Tex> u_normals = {"normals", 0};
    Uni<Tex> u_metallicRoughness = {"metallicRoughness", 1};
    Uni<Tex> u_depth = {"depth", 2};
    Uni<Tex> u_baseColor = {"deferrdBaseColor", 0};
    Uni<Tex> u_lightBuffer = {"lightBuffer", 1};
    Uni<Tex> u_emissionBuffer = {"emissionBuffer", 3};
    Uni<glm::vec4> u_ambient = {"ambient", glm::vec4{0.0f}};
    Uni<Tex> u_normalsCombine = {"normalsCombine", 4};
    const bgfx::Caps *caps;
  } deferrd;

  bgfx::ProgramHandle geom;
  bgfx::ProgramHandle pointLight;
  bgfx::ProgramHandle spotlight;
  bgfx::ProgramHandle combine;
  bgfx::ProgramHandle imgProg;
  GlyphInfo blackGlyph;
};
