#include "test.hpp"
#include "get-natives.hpp"
#include "load-program.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

Test::Test(sdl::Window &aWin, int aW, int aH)
  : win(aWin),
    w(aW),
    h(aH),
    car(assets.get<Mesh>("assets/car.fbx/Car.001")),
    transUniform(bgfx::createUniform("trans", bgfx::UniformType::Mat4)),
    viewPosUniform(bgfx::createUniform("viewPos", bgfx::UniformType::Vec4)),
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

    // modelMat = glm::rotate(modelMat, glm::radians(-20 * secs), glm::vec3(0.0f, 1.0f, 0.0f));
    // modelMat = glm::rotate(modelMat, glm::radians(-5 * secs), glm::vec3(1.0f, 0.0f, 0.0f));

    bgfx::setTransform(&modelMat);

    bgfx::setState(BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | /*BGFX_STATE_CULL_CCW |*/
                   BGFX_STATE_MSAA);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(50.f * secs), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(1., 1080. / 1920., 1.));
    bgfx::setUniform(transUniform, &trans);
    bgfx::setUniform(viewPosUniform, &camPos);
    // auto &mat = assets.get<Mat>(car.mat);
    // mat.arm();
    car.arm();
    bgfx::submit(0, prog);
  }

  bgfx::frame();
}

Test::~Test()
{
  bgfx::destroy(prog);
  bgfx::destroy(viewPosUniform);
  bgfx::destroy(transUniform);
}
