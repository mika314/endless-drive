#pragma once
#include "cube-atlas.hpp"
#include "font-manager.hpp"
#include "scene.hpp"
#include "text-buffer-manager.hpp"
#include "uni.hpp"
#include <bgfx/bgfx.h>
#include <sdlpp/sdlpp.hpp>
#include <unordered_map>

inline void hashCombine(std::size_t &seed, std::size_t v)
{
  seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

class Render
{
public:
  Render(int w, int h);
  ~Render();
  auto getDimensions(const Font &font, float sz, const std::string &text) -> glm::vec2;
  auto render(const Scene &) -> void;
  auto setCamPos(glm::vec3) -> void;
  auto setCamRot(glm::vec3) -> void;
  auto resize(int newW, int newH) -> void;

  struct MeshIn
  {
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
    class Material *mat;
    glm::mat4 trans;
  };
  auto operator()(const MeshIn &) -> void;

  struct PointLightIn
  {
    glm::mat4 trans;
    glm::vec3 color;
  };
  auto operator()(const PointLightIn &) -> void;
  struct SpotlightIn
  {
    glm::mat4 trans;
    glm::vec3 color;
    float angle;
  };
  auto operator()(const SpotlightIn &) -> void;

  struct TextIn
  {
    std::string text;
    const Font &font;
    float sz;
    glm::vec3 color;
    glm::mat4 trans;
  };
  auto operator()(const TextIn &) -> void;

  struct ImgIn
  {
    glm::mat4 trans;
    const Tex &tex;
  };
  auto operator()(const ImgIn &) -> void;
  auto getWidth() const -> int;
  auto getHeight() const -> int;

private:
  int w;
  int h;
  Atlas atlas;
  TextBufferManager textBufferManager;
  TextBufferHandle textBuffer;
  glm::vec3 camPos = {0.f, -5.f, 1.8f};
  glm::vec3 camRot = {-0.3f, 0.0f, 0.0f};
  Uni<glm::vec4> u_camPos = "camPos";
  Uni<glm::mat4> u_mtx = "mtx";
  Uni<glm::mat4> u_projViewCombine = "projViewCombine";
  Uni<glm::vec4> u_time = "time";
  Uni<Tex> u_baseColorTex = {"baseColorTex", 0};
  Uni<Tex> u_metallicTex = {"metallicTex", 1};
  Uni<Tex> u_roughnessTex = {"roughnessTex", 2};
  Uni<Tex> u_emissionTex = {"emissionTex", 3};
  Uni<glm::vec4> u_settings = "settings";
  Uni<glm::vec4> u_baseColor = "baseColor";
  Uni<glm::vec4> u_metallic = "metallic";
  Uni<glm::vec4> u_roughness = "roughness";
  Uni<glm::vec4> u_emission = "emission";
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
    auto operator=(const Deferrd &) = delete;
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
  bgfx::ProgramHandle geomInstanced;
  bgfx::ProgramHandle pointLight;
  bgfx::ProgramHandle spotlight;
  bgfx::ProgramHandle combine;
  bgfx::ProgramHandle imgProg;
  GlyphInfo blackGlyph;
  struct MeshKey
  {
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
    const Material *mat;
    auto operator==(const MeshKey &o) const -> bool
    {
      return vbh.idx == o.vbh.idx && ibh.idx == o.ibh.idx && mat == o.mat;
    }
  };
  struct MeshKeyHasher
  {
    std::size_t operator()(const MeshKey &key) const
    {
      std::size_t seed = 0;
      hashCombine(seed, std::hash<uint16_t>{}(key.vbh.idx));
      hashCombine(seed, std::hash<uint16_t>{}(key.ibh.idx));
      hashCombine(seed, std::hash<const Material *>{}(key.mat));
      return seed;
    }
  };
  std::unordered_map<MeshKey, std::vector<glm::mat4>, MeshKeyHasher> geomRenderData;

  struct Light
  {
    glm::mat4 trans;
    glm::vec3 color;
    float angle;
    bgfx::ProgramHandle prog;
  };
  std::vector<Light> lightRenderData;
  std::vector<std::function<auto()->void>> uiRenderData;
};
