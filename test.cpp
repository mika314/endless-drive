#include "test.hpp"
#include "get-natives.hpp"
#include "load-program.hpp"
#include "mat.hpp"
#include "tex.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

Test::Test(sdl::Window &aWin, int aW, int aH)
  : win(aWin),
    w(aW),
    h(aH),
    car(assets.get<Mesh>("data/car.gltf/Car", assets)),
    u_trans(bgfx::createUniform("trans", bgfx::UniformType::Mat4)),
    u_camPos(bgfx::createUniform("camPos", bgfx::UniformType::Vec4)),
    u_baseColorTex(bgfx::createUniform("baseColorTex", bgfx::UniformType::Sampler)),
    u_metallicTex(bgfx::createUniform("metallicTex", bgfx::UniformType::Sampler)),
    u_roughnessTex(bgfx::createUniform("roughnessTex", bgfx::UniformType::Sampler)),
    u_settings(bgfx::createUniform("settings", bgfx::UniformType::Vec4)),
    u_baseColor(bgfx::createUniform("baseColor", bgfx::UniformType::Vec4)),
    u_metallic(bgfx::createUniform("metallic", bgfx::UniformType::Vec4)),
    u_roughness(bgfx::createUniform("roughness", bgfx::UniformType::Vec4)),
    prog(loadProgram("test-vs", "test-fs"))
{
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x505050ff, 1.0f, 0);
  bgfx::setViewRect(0, 0, 0, w, h);
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

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), 1.f * w / h, 0.1f, 100.0f);

    bgfx::setViewTransform(0, &view, &projection);
  }

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
    bgfx::setUniform(u_camPos, &camPos);
    // auto &mat = assets.get<Mat>(car.mat);
    // mat.arm();
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

    bgfx::submit(0, prog);
  }

  bgfx::frame();
}

Test::~Test()
{
  bgfx::destroy(prog);
  bgfx::destroy(u_roughness);
  bgfx::destroy(u_metallic);
  bgfx::destroy(u_baseColor);
  bgfx::destroy(u_settings);
  bgfx::destroy(u_roughnessTex);
  bgfx::destroy(u_metallicTex);
  bgfx::destroy(u_baseColorTex);
  bgfx::destroy(u_camPos);
  bgfx::destroy(u_trans);
}
