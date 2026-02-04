#include "assets.hpp"
#include "consts.hpp"
#include "game-over.hpp"
#include "gameplay.hpp"
#include "get-natives.hpp"
#include "master-speaker.hpp"
#include "render.hpp"
#include "sample.hpp"
#include "sound-wave.hpp"
#include "title-screen.hpp"
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
  srand(time(nullptr));
  auto init = sdl::Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  auto win = sdl::Window{"Endless Drive", 0, 0, width, height, SDL_WINDOW_FULLSCREEN_DESKTOP};

  auto bgfxInit = BgfxInit{win, width, height};
  bgfx::setDebug(BGFX_DEBUG_TEXT);

  auto assets = Assets{};
  auto render = Render{width, height};
  auto masterSpeaker = MasterSpeaker{};
  auto coinSound = Sample{masterSpeaker, assets.get<SoundWave>("coin"), .33, 0.0};

  for (;;)
  {
    switch (TitleScreen{assets, render, masterSpeaker}.run())
    {
    case TitleScreen::Opt::gameplay:
      coinSound.play();
      GameOver(assets, render, masterSpeaker, Gameplay(assets, render, masterSpeaker).run()).run();
      break;
    case TitleScreen::Opt::settings: break;
    case TitleScreen::Opt::quit: return 0;
    }
  }
}
