#include "render.hpp"
#include "get-natives.hpp"
#include "load-program.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "tex.hpp"
#include <bx/math.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <log/log.hpp>

static const auto geomRenderPass = 0;
static const auto lightRenderPass = 1;
static const auto combineRenderPass = 2;
static const auto uiRenderPass = 3;

namespace
{
  struct PosTexCoord0Vertex
  {
    glm::vec3 p;
    glm::vec2 uv;
    static bgfx::VertexLayout ms_layout;
  };

  bgfx::VertexLayout PosTexCoord0Vertex::ms_layout = []() {
    auto r = bgfx::VertexLayout{};
    r.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
    return r;
  }();

  auto screenSpaceQuad(bool _originBottomLeft, float _width = 1.0f, float _height = 1.0f) -> void
  {
    if (3 != bgfx::getAvailTransientVertexBuffer(3, PosTexCoord0Vertex::ms_layout))
      return;

    bgfx::TransientVertexBuffer vb;
    bgfx::allocTransientVertexBuffer(&vb, 3, PosTexCoord0Vertex::ms_layout);
    PosTexCoord0Vertex *vertex = (PosTexCoord0Vertex *)vb.data;

    const auto minx = -_width;
    const auto maxx = _width;
    const auto miny = 0.0f;
    const auto maxy = _height * 2.f;

    const auto minu = -1.f;
    const auto maxu = 1.f;

    float minv = 0.0f;
    float maxv = 2.f;

    if (_originBottomLeft)
    {
      float temp = minv;
      minv = maxv;
      maxv = temp;

      minv -= 1.0f;
      maxv -= 1.0f;
    }

    vertex[0] = {glm::vec3{minx, miny, 0.0f}, glm::vec2{minu, minv}};
    vertex[1] = {glm::vec3{maxx, miny, 0.0f}, glm::vec2{maxu, minv}};
    vertex[2] = {glm::vec3{maxx, maxy, 0.0f}, glm::vec2{maxu, maxv}};

    bgfx::setVertexBuffer(0, &vb);
  }
} // namespace

Render::Render(int aW, int aH)
  : w(aW),
    h(aH),
    atlas(512),
    textBuffer(textBufferManager.createTextBuffer(FONT_TYPE_ALPHA, BufferType::Transient)),
    deferrd(w, h),
    geom(loadProgram("geom-vs", "geom-fs")),
    geomInstanced(loadProgram("geom-instanced-vs", "geom-fs")),
    pointLight(loadProgram("point-light-vs", "point-light-fs")),
    spotlight(loadProgram("spotlight-vs", "spotlight-fs")),
    combine(loadProgram("combine-vs", "combine-fs")),
    imgProg(loadProgram("img-uivs", "img-uifs"))
{
  const uint32_t W = 3;
  // Create filler rectangle
  uint8_t buffer[W * W * 4];
  bx::memSet(buffer, 255, W * W * 4);

  blackGlyph.width = W;
  blackGlyph.height = W;

  /// make sure the black glyph doesn't bleed by using a one pixel inner outline
  blackGlyph.regionIndex = atlas.addRegion(W, W, buffer, AtlasRegion::TYPE_GRAY, 1);
  atlas.setBlackGlyphRegionIndex(blackGlyph.regionIndex);
}

auto Render::render(const Scene &scene) -> void
{
  geomRenderData.clear();
  lightRenderData.clear();
  uiRenderData.clear();
  scene.prepareRenderData(*this);

  const auto tmpCamPos = camPos;
  const auto rotationMatrix =
    glm::rotate(glm::mat4{1.0f}, camRot.z, glm::vec3{0.0f, 0.0f, 1.0f}) * // Yaw around Z
    glm::rotate(glm::mat4{1.0f}, camRot.x, glm::vec3{1.0f, 0.0f, 0.0f}) * // Pitch around X
    glm::rotate(glm::mat4{1.0f}, camRot.y, glm::vec3{0.0f, 1.0f, 0.0f});  // Roll around Y
  const auto forwardDir = glm::vec3(rotationMatrix * glm::vec4{0.0f, 1.0f, 0.0f, 0.0f});
  const auto view =
    glm::lookAt(tmpCamPos, tmpCamPos + forwardDir, glm::vec3(0.0f, 0.0f, 1.0f) /* Z-up */);

  const auto proj = glm::perspective(glm::radians(60.0f), 1.f * w / h, 0.1f, 1000.0f);
  const auto viewProj = proj * view;
  const auto mtx = glm::inverse(viewProj);

  deferrd.geom();
  bgfx::setViewTransform(geomRenderPass, &view, &proj);
  u_mtx = mtx;
  u_camPos = glm::vec4{camPos, 1.f};

  for (const auto &instances : geomRenderData)
  {
    if (instances.second.empty())
      continue;
    if (instances.second.size() == 1)
    {
      const auto &instance = instances.second.front();
      bgfx::setTransform(&instance);
    }
    else
    {
      bgfx::InstanceDataBuffer idb;
      if (bgfx::getAvailInstanceDataBuffer(instances.second.size(), sizeof(glm::mat4)) !=
          instances.second.size())
      {
        LOG("No available instance data buffers");
        continue;
      }
      bgfx::allocInstanceDataBuffer(&idb, instances.second.size(), sizeof(glm::mat4));
      memcpy(idb.data, instances.second.data(), instances.second.size() * sizeof(glm::mat4));
      bgfx::setInstanceDataBuffer(&idb);
    }

    bgfx::setVertexBuffer(0, instances.first.vbh);
    bgfx::setIndexBuffer(instances.first.ibh);
    const auto mat = instances.first.mat;

    auto tmpSettings = glm::vec4{};
    if (mat)
    {
      if (std::holds_alternative<glm::vec4>(mat->baseColor))
      {
        tmpSettings.x = 0.0f;
        u_baseColor = std::get<glm::vec4>(mat->baseColor);
      }
      else
      {
        tmpSettings.x = 1.f;
        const auto tex = std::get<Tex *>(mat->baseColor);
        assert(tex);
        u_baseColorTex = *tex;
      }
      if (std::holds_alternative<float>(mat->metallic))
      {
        tmpSettings.y = 0.0f;
        u_metallic = glm::vec4{std::get<float>(mat->metallic)};
      }
      else
      {
        tmpSettings.y = 1.f;
        const auto tex = std::get<Tex *>(mat->metallic);
        assert(tex);
        u_metallicTex = *tex;
      }
      if (std::holds_alternative<float>(mat->roughness))
      {
        tmpSettings.z = 0.0f;
        u_roughness = glm::vec4{std::get<float>(mat->roughness)};
      }
      else
      {
        tmpSettings.z = 1.f;
        const auto tex = std::get<Tex *>(mat->roughness);
        assert(tex);
        u_roughnessTex = *tex;
      }
      if (std::holds_alternative<glm::vec4>(mat->emission))
      {
        tmpSettings.w = 0.0f;
        u_emission = std::get<glm::vec4>(mat->emission);
      }
      else
      {
        tmpSettings.w = 1.f;
        const auto tex = std::get<Tex *>(mat->emission);
        assert(tex);
        u_emissionTex = *tex;
      }
    }
    u_settings = tmpSettings;

    if (instances.second.size() == 1)
      bgfx::submit(geomRenderPass, geom);
    else
      bgfx::submit(geomRenderPass, geomInstanced);
  }

  for (const auto &v : lightRenderData)
  {
    deferrd.light();
    u_lightTrans = v.trans;
    u_lightColor = glm::vec4{v.color, 1.f};
    u_lightAngle = glm::vec4{v.angle};
    bgfx::submit(lightRenderPass, v.prog);
  }

  { // combine render pass
    deferrd.combine();
    u_projViewCombine = viewProj;
    u_mtx = mtx;
    u_time = glm::vec4{1.f * (SDL_GetTicks() % 16777216)};
    bgfx::submit(combineRenderPass, combine);
  }

  // UI Pass setup
  bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);
  {
    const auto uiView = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f),  // Camera is at (0,0,1)
                                    glm::vec3(0.0f, 0.0f, 0.0f),  // Looks at (0,0,0)
                                    glm::vec3(0.0f, 1.0f, 0.0f)); // Up is (0,1,0)
    const auto uiProj =
      glm::ortho(0.0f, 1.f * w, 1.f * h, 0.0f, 0.0f, 100.0f); // 0,w,h,0 for top-left origin
    bgfx::setViewTransform(uiRenderPass, &uiView, &uiProj);
    bgfx::setViewRect(uiRenderPass, 0, 0, w, h);
    bgfx::setViewClear(uiRenderPass, BGFX_CLEAR_NONE);
  }

  textBufferManager.clearTextBuffer(textBuffer);

  for (const auto &v : uiRenderData)
    v();

  s_texColor = atlas.getTextureHandle();
  textBufferManager.submitTextBuffer(textBuffer, uiRenderPass);
}

Render::~Render()
{
  bgfx::destroy(combine);
  bgfx::destroy(spotlight);
  bgfx::destroy(pointLight);
  bgfx::destroy(geomInstanced);
  bgfx::destroy(geom);
  bgfx::destroy(imgProg);
  textBufferManager.destroyTextBuffer(textBuffer);
}

auto Render::operator()(const ImgIn &v) -> void
{
  uiRenderData.emplace_back([v, this]() {
    if (bgfx::getAvailTransientVertexBuffer(4, PosTexCoord0Vertex::ms_layout) == 0 ||
        bgfx::getAvailTransientIndexBuffer(6) == 0) // 2 triangles, 3 indices each = 6 indices
    {
      return;
    }

    bgfx::setTransform(&v.trans);
    bgfx::TransientVertexBuffer vb;
    bgfx::allocTransientVertexBuffer(&vb, 4, PosTexCoord0Vertex::ms_layout);
    PosTexCoord0Vertex *vertex = (PosTexCoord0Vertex *)vb.data;

    bgfx::TransientIndexBuffer ib;
    bgfx::allocTransientIndexBuffer(&ib, 6);
    uint16_t *indices = (uint16_t *)ib.data;

    // Quad vertices
    vertex[0] = {glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec2{0.0f, 0.0f}}; // Top-left
    vertex[1] = {glm::vec3{1.f, 0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}};  // Top-right
    vertex[2] = {glm::vec3{1.f, 1.f, 0.0f}, glm::vec2{1.0f, 1.0f}};   // Bottom-right
    vertex[3] = {glm::vec3{0.0f, 1.f, 0.0f}, glm::vec2{0.0f, 1.0f}};  // Bottom-left

    // Indices for two triangles forming a quad
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    u_imgTex = v.tex;

    bgfx::setVertexBuffer(0, &vb);
    bgfx::setIndexBuffer(&ib);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);
    bgfx::submit(uiRenderPass, imgProg);
  });
}

// TextureHandle bgfx::createTexture2D(
//   uint16_t _width,
//   uint16_t _height,
//   bool _hasMips,
//   uint16_t _numLayers,
//   TextureFormat::Enum _format,
//   uint64_t _flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
//   const Memory *_mem = NULL)

static const auto flags = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT;

static auto depthBufTexFmt()
{
  return bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::D32F, BGFX_TEXTURE_RT | flags)
           ? bgfx::TextureFormat::D32F
           : bgfx::TextureFormat::D24;
}

Render::Deferrd::Deferrd(int w, int h)
  : t_baseColor(
      bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | flags)),
    t_metallicRoughness(
      bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | flags)),
    t_normals(
      bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | flags)),
    t_emission(
      bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | flags)),
    t_depth(bgfx::createTexture2D(w, h, false, 1, depthBufTexFmt(), BGFX_TEXTURE_RT | flags)),
    gBuffer(bgfx::createFrameBuffer(
      gBufferAt.size(),
      [&]() {
        gBufferAt[0].init(t_baseColor);
        gBufferAt[1].init(t_metallicRoughness);
        gBufferAt[2].init(t_normals);
        gBufferAt[3].init(t_emission);
        gBufferAt[4].init(t_depth);
        return gBufferAt.data();
      }(),
      true)),
    t_lightBuffer(
      bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | flags)),
    lightBuffer(bgfx::createFrameBuffer(1, &t_lightBuffer, true)),
    caps(bgfx::getCaps())
{
  bgfx::setViewClear(geomRenderPass, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x505050ff, 1.0f, 0);
  bgfx::setViewRect(geomRenderPass, 0, 0, w, h);
  bgfx::setViewFrameBuffer(geomRenderPass, gBuffer);
  auto proj = glm::ortho(0.0f,  // left
                         1.0f,  // left
                         1.0f,  // bottom (reversed)
                         0.0f,  // top (reversed)
                         0.0f,  // near
                         100.0f // far
  );

  if (!caps->homogeneousDepth)
  {
    // OpenGL uses [-1, 1] for depth, need to adjust
    proj[2][2] *= 0.5f;
    proj[3][2] = proj[3][2] * 0.5f + 0.5f;
  }

  bgfx::setViewFrameBuffer(lightRenderPass, lightBuffer);
  bgfx::setViewClear(lightRenderPass, BGFX_CLEAR_COLOR, 0x000000ff, 1.0f, 0);
  bgfx::setViewRect(lightRenderPass, 0, 0, w, h);
  bgfx::setViewTransform(lightRenderPass, nullptr, &proj);

  bgfx::setViewClear(combineRenderPass, BGFX_CLEAR_COLOR, 0xffffffff, 1.0f, 0);
  bgfx::setViewRect(combineRenderPass, 0, 0, w, h);
  bgfx::setViewTransform(combineRenderPass, nullptr, &proj);
}

Render::Deferrd::~Deferrd()
{
  bgfx::destroy(lightBuffer);
  bgfx::destroy(t_lightBuffer);
  bgfx::destroy(gBuffer);
  bgfx::destroy(t_depth);
  bgfx::destroy(t_emission);
  bgfx::destroy(t_normals);
  bgfx::destroy(t_metallicRoughness);
  bgfx::destroy(t_baseColor);
}

auto Render::Deferrd::geom() -> void
{
  bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                 BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW | BGFX_STATE_MSAA);
}

auto Render::Deferrd::light() -> void
{
  u_normals = t_normals;
  u_metallicRoughness = t_metallicRoughness;
  u_depth = t_depth;
  bgfx::setState(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD);
  screenSpaceQuad(caps->originBottomLeft);
}

auto Render::Deferrd::combine() -> void
{
  u_baseColor = t_baseColor;
  u_lightBuffer = t_lightBuffer;
  u_depth = t_depth;
  u_emissionBuffer = t_emission;
  u_normalsCombine = t_normals;
  u_ambient = glm::vec4{.5f};
  bgfx::setState(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
  screenSpaceQuad(caps->originBottomLeft);
}

auto Render::operator()(const MeshIn &in) -> void
{
  geomRenderData[MeshKey{.vbh = in.vbh, .ibh = in.ibh, .mat = in.mat}].emplace_back(in.trans);
}

auto Render::operator()(const PointLightIn &in) -> void
{
  lightRenderData.emplace_back(
    Light{.trans = in.trans, .color = in.color, .angle = 0.0f, .prog = pointLight});
}

auto Render::operator()(const SpotlightIn &in) -> void
{
  lightRenderData.emplace_back(
    Light{.trans = in.trans, .color = in.color, .angle = in.angle, .prog = spotlight});
}

auto Render::setCamPos(glm::vec3 v) -> void
{
  camPos = v;
}

auto Render::setCamRot(glm::vec3 v) -> void
{
  camRot = v;
}

auto Render::operator()(const TextIn &v) -> void
{
  uiRenderData.emplace_back([v, this]() {
    const auto pos = v.trans[3];
    textBufferManager.setTextColor(textBuffer,
                                   (static_cast<uint32_t>(v.color.r * 0xff) << 24) |
                                     (static_cast<uint32_t>(v.color.g * 0xff) << 16) |
                                     (static_cast<uint32_t>(v.color.b * 0xff) << 8) | 0xff);
    textBufferManager.setPenPosition(textBuffer, pos.x, pos.y);
    textBufferManager.appendText(atlas, textBuffer, v.font, v.sz, v.text.c_str());
  });
}
