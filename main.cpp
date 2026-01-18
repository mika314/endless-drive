#include "assets.hpp"
#include "get-natives.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "render.hpp"
#include "scene.hpp"
#include <bgfx/platform.h>
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

  auto &car = scene.addVisualNodeFromAssets<Mesh>(assets, "data/car.gltf/Car");
  for (auto x = -2.f; x < 2.f; x += 1.f)
    for (auto y = -2.f; y < 2.f; y += 1.f)
    {
      auto &light = scene.addVisualNode<Light>(glm::vec4{(x + 2.f) / 4.f, (y + 2.f) / 4.f, .5f, 0.0f});
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
  while (!done)
  {
    while (e.poll()) {}
    auto now = SDL_GetTicks();
    car.setRot(glm::vec3{0.0f, 0.0f, now / 1000.f});
    car.setScale(glm::vec3{1.0f, 1.0f, 1 + sin(now / 1000.f)});
    car.setPos(glm::vec3{cos(now / 1000.f), sin(now / 1000.f), 0.0f});

    render.render(scene);
  }
}
