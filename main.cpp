#include "assets.hpp"
#include "get-natives.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "render.hpp"
#include "scene.hpp"
#include <bgfx/platform.h>
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

  std::vector<std::reference_wrapper<VisualNodeRef<Mesh>>> carParts;

  for (auto i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13})
  {

    auto &part = scene.addVisualNodeFromAssets<Mesh>(
      assets, "fancy-car.gltf/SM_vehCar_vehicle06_LOD-" + std::to_string(i));
    carParts.push_back(part);
  }

  for (auto i = -10; i < 100; ++i)
  {
    const auto dx = 10 * sin(i * 0.1f) + 5 * sin(i * 0.2);

    auto &trafficConeR = scene.addVisualNodeFromAssets<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
    trafficConeR.setPos(glm::vec3{dx + 4.f, 2 * i, 0.0f});
    trafficConeR.setScale(glm::vec3{.5f});
    auto &trafficConeL = scene.addVisualNodeFromAssets<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
    trafficConeL.setPos(glm::vec3{dx - 4.f, 2 * i, 0.0f});
    trafficConeL.setScale(glm::vec3{.5f});
  }

  auto &canister = scene.addVisualNodeFromAssets<Mesh>(assets, "canister.gltf/SM_Canister");
  {
    auto i = 2;
    const auto dx = 10.f * sinf(i * 0.1f) + 5.f * sinf(i * 0.2f);
    canister.setPos(glm::vec3{dx, 2.f * i, 0.0f});
  }

  for (auto x = -2.f; x < 2.f; x += 1.f)
    for (auto y = -2.f; y < 2.f; y += 1.f)
    {
      auto &light = scene.addVisualNode<Light>(0.4f * glm::vec3{1.f});
      light.setPos(glm::vec3{x, y, 2});
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
    for (auto &p : carParts)
    {
      p.get().setRot(glm::vec3{0.0f, 0.0f, now / 1000.f});
      p.get().setScale(glm::vec3{1.0f, 1.0f, 1.f + .1f * sin(now / 100.f)});
      p.get().setPos(glm::vec3{cos(now / 1000.f), sin(now / 1000.f), 0.0f});
    }
    canister.setRot(glm::vec3{0.0f, 0.0f, now / 300.f});

    render.render(scene);
    if (cnt++ > 1200)
    {
      auto t1 = SDL_GetTicks();
      LOG("FPS:", cnt * 1000 / (t1 - t0));
      cnt -= 1200;
      t0 = t1;
    }
  }
}
