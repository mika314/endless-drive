#include "test.hpp"
#include "get-natives.hpp"
#include "load-program.hpp"
#include "mat.hpp"
#include "tex.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

static const auto geomRenderPass = 0;
static const auto lightRenderPass = 1;
static const auto combineRenderPass = 2;

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

Test::Test(sdl::Window &aWin, int aW, int aH)
  : win(aWin),
    w(aW),
    h(aH),
    car(assets.get<Mesh>("data/car.gltf/Car", assets)),
    u_trans(bgfx::createUniform("trans", bgfx::UniformType::Mat4)),
    u_camPos(bgfx::createUniform("camPos", bgfx::UniformType::Vec4)),
    u_mtx(bgfx::createUniform("mtx", bgfx::UniformType::Mat4)),
    u_baseColorTex(bgfx::createUniform("baseColorTex", bgfx::UniformType::Sampler)),
    u_metallicTex(bgfx::createUniform("metallicTex", bgfx::UniformType::Sampler)),
    u_roughnessTex(bgfx::createUniform("roughnessTex", bgfx::UniformType::Sampler)),
    u_settings(bgfx::createUniform("settings", bgfx::UniformType::Vec4)),
    u_baseColor(bgfx::createUniform("baseColor", bgfx::UniformType::Vec4)),
    u_metallic(bgfx::createUniform("metallic", bgfx::UniformType::Vec4)),
    u_roughness(bgfx::createUniform("roughness", bgfx::UniformType::Vec4)),
    deferrd(w, h),
    geom(loadProgram("geom-vs", "geom-fs")),
    light(loadProgram("light-vs", "light-fs")),
    combine(loadProgram("combine-vs", "combine-fs")),
    caps(bgfx::getCaps())
{
  bgfx::setViewClear(geomRenderPass, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x505050ff, 1.0f, 0);
  bgfx::setViewRect(geomRenderPass, 0, 0, w, h);
  bgfx::setViewFrameBuffer(geomRenderPass, deferrd.gBuffer);

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

  bgfx::setViewClear(lightRenderPass, BGFX_CLEAR_COLOR, 0xffffffff, 1.0f, 0);
  bgfx::setViewRect(lightRenderPass, 0, 0, w, h);
  bgfx::setViewFrameBuffer(lightRenderPass, deferrd.lightBuffer);
  bgfx::setViewTransform(lightRenderPass, nullptr, &proj);

  bgfx::setViewClear(combineRenderPass, BGFX_CLEAR_COLOR, 0xffffffff, 1.0f, 0);
  bgfx::setViewRect(combineRenderPass, 0, 0, w, h);
  bgfx::setViewTransform(combineRenderPass, nullptr, &proj);
}

auto Test::tick() -> void
{
  {
    const auto view =
      glm::lookAt(camPos,
                  camPos + glm::vec3{glm::rotate(glm::mat4{1.0f}, camYaw, glm::vec3{0.0f, 0.0f, 1.f}) *
                                     glm::rotate(glm::mat4{1.0f}, camPitch, glm::vec3{0.0f, 1.f, 0.0f}) *
                                     glm::vec4{1.f, 0.f, 0.f, 1.f}},
                  glm::vec3(0.0, 0.0, 1.0));

    glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1.f * w / h, 0.1f, 100.0f);
    bgfx::setViewTransform(geomRenderPass, &view, &proj);
    const auto viewProj = proj * view;
    glm::mat4 mtx = glm::inverse(viewProj);
    bgfx::setUniform(u_mtx, &mtx);
  }

  bgfx::setUniform(u_camPos, &camPos);

  auto i = 0;
  for (auto cubePosition : std::array{
         glm::vec3(0.0f, 0.0f, 0.0f),
         // glm::vec3(2.0f, 5.0f, -15.0f),
         // glm::vec3(-1.5f, -2.2f, -2.5f),
         // glm::vec3(-3.8f, -2.0f, -12.3f),
         // glm::vec3(2.4f, -0.4f, -3.5f),
         // glm::vec3(-1.7f, 3.0f, -7.5f),
         // glm::vec3(1.3f, -2.0f, -2.5f),
         // glm::vec3(1.5f, 2.0f, -2.5f),
         // glm::vec3(1.5f, 0.2f, -1.5f),
         // glm::vec3(-1.3f, 1.0f, -1.5f)
       })
  {
    auto modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, cubePosition);
    float angle = 20.0f * i++;
    modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

    const auto secs = SDL_GetTicks() / 1000.f;

    modelMat = glm::rotate(modelMat, glm::radians(-20 * secs), glm::vec3(0.0f, 0.0f, 1.0f));
    // modelMat = glm::rotate(modelMat, glm::radians(-5 * secs), glm::vec3(1.0f, 0.0f, 0.0f));

    bgfx::setTransform(&modelMat);

    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | /*BGFX_STATE_CULL_CCW |*/
                   BGFX_STATE_MSAA);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(50.f * secs), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(1., 1080. / 1920., 1.));
    bgfx::setUniform(u_trans, &trans);
    auto mat = car.arm();
    auto settings = glm::vec4{};
    if (mat)
    {
      if (std::holds_alternative<glm::vec4>(mat->baseColor))
      {
        settings.x = 0.0f;
        bgfx::setUniform(u_baseColor, &std::get<glm::vec4>(mat->baseColor));
      }
      else
      {
        settings.x = 1.f;
        const auto tex = std::get<Tex *>(mat->baseColor);
        assert(tex);
        bgfx::setTexture(0, u_baseColorTex, std::get<Tex *>(mat->baseColor)->h);
      }
      if (std::holds_alternative<float>(mat->metallic))
      {
        settings.y = 0.0f;
        const auto tmp = glm::vec4{std::get<float>(mat->metallic)};
        bgfx::setUniform(u_metallic, &tmp);
      }
      else
      {
        settings.y = 1.f;
        const auto tex = std::get<Tex *>(mat->metallic);
        assert(tex);
        bgfx::setTexture(1, u_metallicTex, std::get<Tex *>(mat->metallic)->h);
      }
      if (std::holds_alternative<float>(mat->roughness))
      {
        settings.z = 0.0f;
        const auto tmp = glm::vec4{std::get<float>(mat->roughness)};
        bgfx::setUniform(u_roughness, &tmp);
      }
      else
      {
        settings.z = 1.f;
        const auto tex = std::get<Tex *>(mat->roughness);
        assert(tex);
        bgfx::setTexture(2, u_roughnessTex, std::get<Tex *>(mat->roughness)->h);
      }
    }
    bgfx::setUniform(u_settings, &settings);

    bgfx::submit(geomRenderPass, geom);
  }

  { // light render pass
    bgfx::setState(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD);
    bgfx::setTexture(0, deferrd.u_normals, deferrd.t_normals);
    bgfx::setTexture(1, deferrd.u_metallicRoughness, deferrd.t_metallicRoughness);
    bgfx::setTexture(2, deferrd.u_depth, deferrd.t_depth);
    bgfx::setState(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
    screenSpaceQuad(caps->originBottomLeft);

    bgfx::submit(lightRenderPass, light);
  }

  { // combine render pass
    bgfx::setTexture(0, deferrd.u_baseColor, deferrd.t_baseColor);
    bgfx::setTexture(1, deferrd.u_lightBuffer, deferrd.t_lightBuffer);
    bgfx::setState(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
    screenSpaceQuad(caps->originBottomLeft);

    bgfx::submit(combineRenderPass, combine);
  }

  bgfx::frame();
}

Test::~Test()
{
  bgfx::destroy(combine);
  bgfx::destroy(light);
  bgfx::destroy(geom);
  bgfx::destroy(u_roughness);
  bgfx::destroy(u_metallic);
  bgfx::destroy(u_baseColor);
  bgfx::destroy(u_settings);
  bgfx::destroy(u_roughnessTex);
  bgfx::destroy(u_metallicTex);
  bgfx::destroy(u_baseColorTex);
  bgfx::destroy(u_mtx);
  bgfx::destroy(u_camPos);
  bgfx::destroy(u_trans);
}

// TextureHandle bgfx::createTexture2D(
//   uint16_t _width,
//   uint16_t _height,
//   bool _hasMips,
//   uint16_t _numLayers,
//   TextureFormat::Enum _format,
//   uint64_t _flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
//   const Memory *_mem = NULL)

static const auto flags = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT |
                          BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

static auto depthBufTexFmt()
{
  return bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::D32F, BGFX_TEXTURE_RT | flags)
           ? bgfx::TextureFormat::D32F
           : bgfx::TextureFormat::D24;
}

Test::Deferrd::Deferrd(int w, int h)
  : t_baseColor(
      bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | flags)),
    t_metallicRoughness(
      bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | flags)),
    t_normals(
      bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | flags)),
    t_depth(bgfx::createTexture2D(w, h, false, 1, depthBufTexFmt(), BGFX_TEXTURE_RT | flags)),
    gBuffer(bgfx::createFrameBuffer(
      gBufferAt.size(),
      [&]() {
        gBufferAt[0].init(t_baseColor);
        gBufferAt[1].init(t_metallicRoughness);
        gBufferAt[2].init(t_normals);
        gBufferAt[3].init(t_depth);
        return gBufferAt.data();
      }(),
      true)),
    t_lightBuffer(
      bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT | flags)),
    lightBuffer(bgfx::createFrameBuffer(1, &t_lightBuffer, true)),
    u_baseColor(bgfx::createUniform("baseColor", bgfx::UniformType::Sampler)),
    u_metallicRoughness(bgfx::createUniform("metallicRoughness", bgfx::UniformType::Sampler)),
    u_normals(bgfx::createUniform("normals", bgfx::UniformType::Sampler)),
    u_depth(bgfx::createUniform("depth", bgfx::UniformType::Sampler)),
    u_lightBuffer(bgfx::createUniform("lightBuffer", bgfx::UniformType::Sampler))
{
}

Test::Deferrd::~Deferrd()
{
  bgfx::destroy(u_lightBuffer);
  bgfx::destroy(u_depth);
  bgfx::destroy(u_normals);
  bgfx::destroy(u_metallicRoughness);
  bgfx::destroy(u_baseColor);
  bgfx::destroy(lightBuffer);
  bgfx::destroy(t_lightBuffer);
  bgfx::destroy(gBuffer);
  bgfx::destroy(t_depth);
  bgfx::destroy(t_normals);
  bgfx::destroy(t_metallicRoughness);
  bgfx::destroy(t_baseColor);
}
