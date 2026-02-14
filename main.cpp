#include "assets.hpp"
#include "consts.hpp"
#include "game-over.hpp"
#include "gameplay.hpp"
#include "get-natives.hpp"
#include "master-speaker.hpp"
#include "render.hpp"
#include "sample.hpp"
#include "send.hpp"
#include "settings.hpp"
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
  BgfxInit(sdl::Window &win)
  {
    bgfx::init([&]() {
      int width;
      int height;
      win.getSize(&width, &height);
      auto r = bgfx::Init{};
#ifdef BX_PLATFORM_WINDOWS
      r.type = bgfx::RendererType::Direct3D12;
#else
      r.type = bgfx::RendererType::Vulkan;
#endif
      r.vendorId = BGFX_PCI_ID_NONE; // args.m_pciId;
      r.platformData.nwh = getNativeWindowHandle(win);
      r.platformData.ndt = getNativeDisplayHandle(win);
      r.resolution.width = width;
      r.resolution.height = height;
      r.resolution.reset = ResetFlags;
      return r;
    }());
  }
  ~BgfxInit() { bgfx::shutdown(); }
};

auto main(int /*argc*/, char ** /*argv*/) -> int
{
  srand(time(nullptr));
  auto init = sdl::Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  SDL_ShowCursor(false);

  auto win = sdl::Window{"Endless Drive", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP};

  auto bgfxInit = BgfxInit{win};
  bgfx::setDebug(BGFX_DEBUG_TEXT);

  auto assets = Assets{};
  int width;
  int height;
  win.getSize(&width, &height);
  auto render = Render{width, height};
  auto masterSpeaker = MasterSpeaker{};
  auto musicSend = Send{masterSpeaker};
  auto sfxSend = Send{masterSpeaker};
  auto coinSound = Sample{masterSpeaker, assets.get<SoundWave>("coin"), .33, 0.0};
  auto settings = Settings{assets, render, masterSpeaker, musicSend, sfxSend};

  if (!settings.fullScreen)
  {
    win.setFullscreen(false);
    SDL_SetWindowResizable(win.get(), SDL_TRUE);
    win.setSize(1884, 1060);
  }

  auto titleScreen = TitleScreen{assets, render, sfxSend};
  for (;;)
  {
    switch (titleScreen.run())
    {
    case TitleScreen::Opt::gameplay:
      coinSound.play();
      GameOver{assets, render, sfxSend, Gameplay{assets, render, musicSend, sfxSend, settings}.run()}
        .run();
      break;
    case TitleScreen::Opt::settings: settings.run(win); break;
    case TitleScreen::Opt::quit: return 0;
    }
  }
}
