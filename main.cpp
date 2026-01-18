#include "assets.hpp"
#include "get-natives.hpp"
#include "mesh.hpp"
#include "render.hpp"
#include "scene.hpp"
#include <bgfx/platform.h>
#include <bx/readerwriter.h>
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

//#include <bx/mutex.h>
//#include <bx/thread.h>
//#include <bx/handlealloc.h>

auto main(int argc, char **argv) -> int
{
  auto init = sdl::Init(SDL_INIT_VIDEO);
  int m_width = 1280;
  int m_height = 720;

  auto win = sdl::Window{
    "bgfx", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN};

  bgfx::init([&]() {
    auto r = bgfx::Init{};
    r.type = bgfx::RendererType::OpenGL;
    // .type     = bgfx::RendererType::Vulkan;
    r.vendorId = BGFX_PCI_ID_NONE; // args.m_pciId;
    r.platformData.nwh = getNativeWindowHandle(win);
    r.platformData.ndt = getNativeDisplayHandle(win);
    r.resolution.width = m_width;
    r.resolution.height = m_height;
    r.resolution.reset = BGFX_RESET_VSYNC;
    return r;
  }());

  bgfx::renderFrame();
  {
    auto render = Render{win, m_width, m_height};
    auto assets = Assets{};
    auto scene = Scene{assets};
    auto &car = scene.addVisualNode<Mesh>("data/car.gltf/Car");

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

  bgfx::shutdown();
}
