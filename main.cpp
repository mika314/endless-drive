#include "assets.hpp"
#include "get-natives.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "render.hpp"
#include "scene.hpp"
#include <bgfx/platform.h>
#include <list>
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
#include <wayland-egl.h>
#endif
#elif BX_PLATFORM_WINDOWS
#define SDL_MAIN_HANDLED
#endif

#if defined(None) // X11 defines this...
#undef None
#endif // defined(None)

class BgfxInit
{
public:
  BgfxInit(sdl::Window &win, int width, int height)
  {
    bgfx::init([&]() {
      auto r = bgfx::Init{};
      r.type = bgfx::RendererType::OpenGL;
      // .type     = bgfx::RendererType::Vulkan;
      r.vendorId = BGFX_PCI_ID_NONE; // args.m_pciId;
      r.platformData.nwh = getNativeWindowHandle(win);
      r.platformData.ndt = getNativeDisplayHandle(win);
      r.resolution.width = width;
      r.resolution.height = height;
      r.resolution.reset = BGFX_RESET_VSYNC;
      return r;
    }());
  }
  ~BgfxInit() { bgfx::shutdown(); }
};

auto main(int /*argc*/, char ** /*argv*/) -> int
{
  auto init = sdl::Init(SDL_INIT_VIDEO);
  const auto width = 1280;
  const auto height = 720;

  auto win = sdl::Window{
    "bgfx", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN};

  auto bgfxInit = BgfxInit{win, width, height};

  auto render = Render{win, width, height};
  auto assets = Assets{};
  auto scene = Scene{};

  auto &car = scene.addVisualNode<Mesh>(assets, "fancy-car.gltf/SM_vehCar_vehicle06_LOD");

  std::list<std::reference_wrapper<BaseVisualNode>> tmpMesh;

  for (auto i = -10; i < 100; ++i)
  {
    const auto dx = 10 * sin(i * 0.1f) + 5 * sin(i * 0.2);
    {
      auto &mesh = scene.addVisualNode<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
      mesh.setPos(glm::vec3{dx + 4.f, 2 * i, 0.0f});
      mesh.setScale(glm::vec3{.5f});
      tmpMesh.push_back(mesh);
    }
    {
      auto &mesh = scene.addVisualNode<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
      mesh.setPos(glm::vec3{dx - 4.f, 2 * i, 0.0f});
      mesh.setScale(glm::vec3{.5f});
      tmpMesh.push_back(mesh);
    }
    if (i % 5 == 0)
    {
      {
        auto &mesh = scene.addVisualNode<Mesh>(assets, "street-light.gltf/SM_SingleLight_01");
        mesh.setPos(glm::vec3{dx + 5.f, 2 * i, 0.0f});
        mesh.setRot(glm::vec3{0.0f, 0.0f, 3.1415926f});
        tmpMesh.push_back(mesh);
      }
      {
        auto &mesh = scene.addVisualNode<Mesh>(assets, "street-light.gltf/SM_SingleLight_01");
        mesh.setPos(glm::vec3{dx - 5.f, 2 * i, 0.0f});
        tmpMesh.push_back(mesh);
      }
    }
  }

  {
    auto &mesh = car.addVisualNode<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
    mesh.setPos(glm::vec3{0.0f, -1.5f, 1.5f});
    mesh.setScale(glm::vec3{.5f});
  }

  {
    auto &mesh = car.addVisualNode<Light>(0.4f * glm::vec3{1.f});
    mesh.setPos(glm::vec3{.596434f, 2.1f, .704822f});
  }
  {
    auto &mesh = car.addVisualNode<Light>(0.4f * glm::vec3{1.f});
    mesh.setPos(glm::vec3{-.596434f, 2.1f, .704822f});
  }

  auto &canister = scene.addVisualNode<Mesh>(assets, "canister.gltf/SM_Canister");
  {
    auto i = 2;
    const auto dx = 10.f * sinf(i * 0.1f) + 5.f * sinf(i * 0.2f);
    canister.setPos(glm::vec3{dx, 2.f * i, 0.0f});
  }

  {
    auto &mesh = scene.addVisualNode<Mesh>(assets, "tires-bunch.gltf/SM_TiresBunch_02");
    auto i = 3;
    const auto dx = 10.f * sinf(i * 0.1f) + 5.f * sinf(i * 0.2f);
    mesh.setPos(glm::vec3{dx - 1.f, 2.f * i, 0.0f});
  }

  for (auto x = -2.f; x < 2.f; x += 1.f)
    for (auto y = -2.f; y < 2.f; y += 1.f)
    {
      auto &mesh = scene.addVisualNode<Light>(0.4f * glm::vec3{1.f});
      mesh.setPos(glm::vec3{x, y, 2});
    }

  auto done = false;
  auto e = sdl::EventHandler{};
  e.quit = [&done](const SDL_QuitEvent &) { done = true; };
  e.keyDown = [&](const SDL_KeyboardEvent &e) {
    switch (e.keysym.sym)
    {
    case SDLK_q: done = true; break;
    }
  };

  auto t0 = SDL_GetTicks();
  auto cnt = 0;

  while (!done)
  {
    while (e.poll()) {}
    auto now = SDL_GetTicks();
    const auto carYOffset = now / 1'000.f;
    car.setRot(glm::vec3{0.0f, 0.0f, now / 1000.f});
    car.setScale(glm::vec3{1.0f, 1.0f, 1.f + .1f * sin(now / 100.f)});
    car.setPos(glm::vec3{cos(now / 1000.f), sin(now / 1000.f) + carYOffset, 0.0f});
    canister.setRot(glm::vec3{0.0f, 0.0f, now / 300.f});

    const auto r = 5.f + 3.f * sin(now / 12000.f);
    render.setCamPos(glm::vec3{r * sin(-(now / 6000.f)), -r * cos(-(now / 6000.f)) + carYOffset, 1.8f});
    render.setCamRot(glm::vec3{-0.3f, 0.0f, -(now / 6000.f)});
    render.render(scene);
    while (!tmpMesh.empty() && tmpMesh.front().get().getPos().y < carYOffset - 6.f)
    {
      scene.remove(tmpMesh.front());
      tmpMesh.pop_front();
    }
    auto t1 = SDL_GetTicks();
    ++cnt;
    if (t1 - t0 >= 10'000)
    {
      LOG("FPS:", cnt * 1000. / (t1 - t0));
      cnt = 0;
      t0 = t1;
    }
  }
}
