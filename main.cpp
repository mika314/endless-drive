#include "assets.hpp"
#include "canister.hpp"
#include "car.hpp"
#include "get-natives.hpp"
#include "get-road-offset.hpp"
#include "mesh.hpp"
#include "point-light.hpp"
#include "render.hpp"
#include "scene.hpp"
#include "spotlight.hpp"
#include "street-light.hpp"
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
  const auto width = 1920;
  const auto height = 1080;

  auto win = sdl::Window{
    "bgfx", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN};

  auto bgfxInit = BgfxInit{win, width, height};

  auto render = Render{win, width, height};
  auto assets = Assets{};
  auto scene = Scene{};

  auto &car = scene.addNode<Car>(assets);

  std::list<std::reference_wrapper<BaseVisualNode>> tmpNodes;

  auto addRoadMeshes = [&](const auto i) {
    const auto y = 2 * i;
    const auto dx = getRoadOffset(y);
    {
      auto &node = scene.addVisualNode<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
      node.setPos({dx + 4.f, y, 0.0f});
      node.setScale(glm::vec3{.5f});
      tmpNodes.push_back(node);
    }
    {
      auto &node = scene.addVisualNode<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
      node.setPos({dx - 4.f, y, 0.0f});
      node.setScale(glm::vec3{.5f});
      tmpNodes.push_back(node);
    }
    if (i % 5 == 0)
    {
      {
        auto &node = scene.addNode<StreetLight>(assets);
        node.setPos({dx + 5.f, y, 0.0f});
        node.setRot({0.0f, 0.0f, -3.1415926f / 2});
        tmpNodes.push_back(node);
      }
      {
        auto &node = scene.addNode<StreetLight>(assets);
        node.setPos({dx - 5.f, y, 0.0f});
        node.setRot({0.0f, 0.0f, 3.1415926f / 2});
        tmpNodes.push_back(node);
      }
    }
    if (rand() % 10 == 0)
    {
      auto &node = scene.addVisualNode<Mesh>(assets, "tires-bunch.gltf/SM_TiresBunch_02");
      auto offset = 0.0f;
      switch (rand() % 3)
      {
      case 0: offset = 0.0f; break;
      case 1: offset = -2.25f; break;
      case 2: offset = 2.25f; break;
      };
      node.setPos({dx + offset, y, 0.0f});
      tmpNodes.push_back(node);
    }
    else if (rand() % 2 == 0)
    {
      auto &node = scene.addNode<Canister>(assets);
      auto offset = 0.0f;
      switch (rand() % 3)
      {
      case 0: offset = 0.0f; break;
      case 1: offset = -2.25f; break;
      case 2: offset = 2.25f; break;
      };
      node.setPos({dx + offset, y, 0.0f});
      tmpNodes.push_back(node);
    }
  };

  for (auto i = -10; i < 100; ++i)
    addRoadMeshes(i);

  auto roadIdx = 100;

  auto &floor = scene.addVisualNode<Mesh>(assets, "floor.gltf/Floor");

  auto done = false;
  auto e = sdl::EventHandler{};
  e.quit = [&done](const SDL_QuitEvent &) { done = true; };
  e.keyDown = [&](const SDL_KeyboardEvent &e) {
    switch (e.keysym.sym)
    {
    case SDLK_q: done = true; break;
    case SDLK_LEFT:
      if (car.currentLane > -1)
        --car.currentLane;
      break;
    case SDLK_RIGHT:
      if (car.currentLane < 1)
        ++car.currentLane;
      break;
    }
  };

  auto t0 = SDL_GetTicks();
  auto cnt = 0;

  auto dt0 = t0;

  while (!done)
  {
    while (e.poll()) {}
    const auto now = SDL_GetTicks();
    const auto carYOffset = Car::desiredY();
    floor.setPos({0.0f, carYOffset, 0.0f});

    scene.tick((now - dt0) / 1000.f);
    dt0 = now;

    render.setCamPos({0.0f, carYOffset - 12.f, 2.3f});
    render.setCamRot({0.0f, 0.0f, 0.0f});
    render.render(scene);
    while (!tmpNodes.empty() && tmpNodes.front().get().getPos().y < carYOffset - 24.f)
    {
      scene.remove(tmpNodes.front());
      tmpNodes.pop_front();
    }
    for (; roadIdx * 2 - 200 < carYOffset; ++roadIdx)
      addRoadMeshes(roadIdx);
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
