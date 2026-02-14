#include "settings.hpp"
#include "consts.hpp"
#include "font.hpp"
#include "img-node.hpp"
#include "label-node.hpp"
#include "master-speaker.hpp"
#include "render.hpp"
#include "sample.hpp"
#include "scene.hpp"
#include "sound-wave.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <json-ser/json-ser.hpp>
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>

// Converts a linear slider value (0.0 to 1.0) to audio gain.
// Uses a decibel-based logarithmic mapping for natural hearing response.
static auto sliderToGain(float x) -> float
{
  if (x <= 0.0f)
    return 0.0f;
  const auto minDb = -45.f;
  const auto db = (x * -minDb) + minDb;
  return std::pow(10.f, db / 20.f);
}

Settings::Settings(Assets &aAssets,
                   sdl::Window &aWin,
                   Render &aRender,
                   Sink &aMaster,
                   Sink &aMusicSend,
                   Sink &aSfxSend)
  : assets(aAssets),
    win(aWin),
    render(aRender),
    master(aMaster),
    musicSend(aMusicSend),
    sfxSend(aSfxSend),
    prefPath([]() {
      auto cPrefPath = SDL_GetPrefPath("1eu", "endless-drive");
      auto r = std::string{cPrefPath};
      SDL_free(cPrefPath);
      return r;
    }())
{
  if (auto f = std::ifstream{prefPath + "settings.json", std::ios::binary})
    jsonDeser(f, *this);

  if (!fullScreen)
  {
    win.setFullscreen(false);
    SDL_SetWindowResizable(win.get(), SDL_TRUE);
    win.setSize(1884, 1060);
  }

  master.gain = sliderToGain(masterVolume);
  musicSend.gain = sliderToGain(music);
  sfxSend.gain = sliderToGain(sfx);
}

auto Settings::run() -> void
{
  if (auto f = std::ifstream{prefPath + "settings.json", std::ios::binary})
    jsonDeser(f, *this);
  auto scene = Scene{nullptr};
  auto coinSound = Sample{sfxSend, assets.get<SoundWave>("coin"), .33, 0.0};
  auto done = false;
  auto e = sdl::EventHandler{};
  e.quit = [&done](const SDL_QuitEvent &) { done = true; };
  e.windowEvent = [&](const SDL_WindowEvent &sdl_e) {
    if (sdl_e.event == SDL_WINDOWEVENT_RESIZED || sdl_e.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    {
      const auto w = sdl_e.data1;
      const auto h = sdl_e.data2;
      bgfx::reset(w, h, ResetFlags);
      render.resize(w, h);
    }
  };

  auto exit = [&]() { done = true; };
  auto apply = [&]() {
    switch (selection)
    {
    case 3:
      coinSound.play();
      fullScreen = !fullScreen;
      if (fullScreen)
        win.setFullscreen(true);
      else
      {
        win.setFullscreen(false);
        SDL_SetWindowResizable(win.get(), SDL_TRUE);
        win.setSize(1884, 1060);
      }
      {
        int w, h;
        win.getSize(&w, &h);
        bgfx::reset(w, h, ResetFlags);
        render.resize(w, h);
      }
      break;
    case 4:
      coinSound.play();
      showFps = !showFps;
      break;
    }
  };

  auto up = [&]() {
    coinSound.play();
    selection = (selection + 5 - 1) % 5;
  };

  auto down = [&]() {
    coinSound.play();
    selection = (selection + 1) % 5;
  };

  auto dec = [&]() {
    switch (selection)
    {
    case 0:
      masterVolume = std::max(0.0f, masterVolume - 0.05f);
      master.gain = sliderToGain(masterVolume);
      coinSound.play();
      break;
    case 1:
      music = std::max(0.0f, music - 0.05f);
      musicSend.gain = sliderToGain(music);
      coinSound.play();
      break;
    case 2:
      sfx = std::max(0.0f, sfx - 0.05f);
      sfxSend.gain = sliderToGain(sfx);
      coinSound.play();
      break;
    }
  };

  auto inc = [&]() {
    switch (selection)
    {
    case 0:
      masterVolume = std::min(1.f, masterVolume + 0.05f);
      master.gain = sliderToGain(masterVolume);
      coinSound.play();
      break;
    case 1:
      music = std::min(1.f, music + 0.05f);
      musicSend.gain = sliderToGain(music);
      coinSound.play();
      break;
    case 2:
      sfx = std::min(1.f, sfx + 0.05f);
      sfxSend.gain = sliderToGain(sfx);
      coinSound.play();
      break;
    }
  };

  e.keyDown = [&](const SDL_KeyboardEvent &e) {
    switch (e.keysym.sym)
    {
    case SDLK_ESCAPE: exit(); break;
    case SDLK_SPACE:
    case SDLK_RETURN: apply(); break;
    case SDLK_UP: up(); break;
    case SDLK_DOWN: down(); break;
    case SDLK_LEFT: dec(); break;
    case SDLK_RIGHT: inc(); break;
    }
  };

  e.controllerButtonDown = [&](const auto &e) {
    switch (e.button)
    {
    case SDL_CONTROLLER_BUTTON_START:
    case SDL_CONTROLLER_BUTTON_B: exit(); break;
    case SDL_CONTROLLER_BUTTON_A: apply(); break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: up(); break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: down(); break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: dec(); break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: inc(); break;
    }
  };

  const auto width = render.getWidth();
  const auto height = render.getHeight();

  auto y = height / 16.f;
  auto masterVolumePos = glm::vec2{};
  auto musicPos = glm::vec2{};
  auto sfxPos = glm::vec2{};
  auto minX = 1.f * width;
  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Settings",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 200});
    const auto dim = node.getDimensions(render);
    node.setPos(glm::vec2{(width - dim.x) / 2.f, height / 16.f});
    y += dim.y;
  }
  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Master Volume",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    auto &img = scene.addNode<ImgNode>(Img{.tex = assets.get<Tex>("progress-bar"),
                                           .sz = glm::vec2{200.f, 50.f},
                                           .pivot = glm::vec2{0.0f, 0.0f}});
    masterVolumePos =
      glm::vec2{(width - (dim.x + 250.f)) / 2.f + 50.f + dim.x, y + (dim.y + 50.f) / 2.f + 40.f};
    img.setPos(masterVolumePos);
    node.setPos(glm::vec2{(width - (dim.x + 250.f)) / 2.f, y});
    y += dim.y;
    minX = std::min(minX, node.getPos().x);
  }
  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Music",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    auto &img = scene.addNode<ImgNode>(Img{.tex = assets.get<Tex>("progress-bar"),
                                           .sz = glm::vec2{200.f, 50.f},
                                           .pivot = glm::vec2{0.0f, 0.0f}});
    musicPos =
      glm::vec2{(width - (dim.x + 250.f)) / 2.f + 50.f + dim.x, y + (dim.y + 50.f) / 2.f + 40.f};
    img.setPos(musicPos);
    node.setPos(glm::vec2{(width - (dim.x + 250.f)) / 2.f, y});
    y += dim.y;
    minX = std::min(minX, node.getPos().x);
  }
  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "SFX",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    auto &img = scene.addNode<ImgNode>(Img{.tex = assets.get<Tex>("progress-bar"),
                                           .sz = glm::vec2{200.f, 50.f},
                                           .pivot = glm::vec2{0.0f, 0.0f}});

    sfxPos = glm::vec2{(width - (dim.x + 250.f)) / 2.f + 50.f + dim.x, y + (dim.y + 50.f) / 2.f + 40.f};
    img.setPos(sfxPos);
    node.setPos(glm::vec2{(width - (dim.x + 250.f)) / 2.f, y});
    y += dim.y;
    minX = std::min(minX, node.getPos().x);
  }

  auto fullScreenCheckMark = [&]() {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Full Screen",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    auto &img = node.addNode<ImgNode>(Img{
      .tex = assets.get<Tex>("checkbox"), .sz = glm::vec2{90.f, 90.f}, .pivot = glm::vec2{1.0f, 0.0f}});
    img.setPos(glm::vec2{-20.0f, 100.f});
    auto &checkMark = node.addNode<ImgNode>(Img{.tex = assets.get<Tex>("check-mark"),
                                                .sz = glm::vec2{90.f, 90.f},
                                                .pivot = glm::vec2{1.0f, 0.0f}});
    checkMark.setPos(glm::vec2{-20.0f, 100.f});
    node.setPos(glm::vec2{(width - dim.x - 64.f - 10.f) / 2.f + 64.f + 10.f, y});
    y += dim.y;
    minX = std::min(minX, node.getPos().x - 20.f - 90.f);
    return &checkMark;
  }();

  auto selectionHeight = 0.0f;
  auto showFpsCheckMark = [&]() {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Show FPS",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    auto &img = node.addNode<ImgNode>(Img{
      .tex = assets.get<Tex>("checkbox"), .sz = glm::vec2{90.f, 90.f}, .pivot = glm::vec2{1.0f, 0.0f}});
    img.setPos(glm::vec2{-20.0f, 100.f});
    auto &checkMark = node.addNode<ImgNode>(Img{.tex = assets.get<Tex>("check-mark"),
                                                .sz = glm::vec2{90.f, 90.f},
                                                .pivot = glm::vec2{1.0f, 0.0f}});
    checkMark.setPos(glm::vec2{-20.0f, 100.f});
    node.setPos(glm::vec2{(width - dim.x - 64.f - 10.f) / 2.f + 64.f + 10.f, y});
    y += dim.y;
    selectionHeight = dim.y;
    minX = std::min(minX, node.getPos().x - 20.f - 90.f);
    return &checkMark;
  }();
  auto &masterVolumeThumb = scene.addNode<ImgNode>(
    Img{.tex = assets.get<Tex>("thumb"), .sz = glm::vec2{50.f, 50.f}, .pivot = glm::vec2{.5f, 0.0f}});
  auto &musicThumb = scene.addNode<ImgNode>(
    Img{.tex = assets.get<Tex>("thumb"), .sz = glm::vec2{50.f, 50.f}, .pivot = glm::vec2{.5f, 0.0f}});
  auto &sfxThumb = scene.addNode<ImgNode>(
    Img{.tex = assets.get<Tex>("thumb"), .sz = glm::vec2{50.f, 50.f}, .pivot = glm::vec2{.5f, 0.0f}});
  auto &selector = scene.addNode<ImgNode>(Img{
    .tex = assets.get<Tex>("selector"), .sz = glm::vec2{100.f, 100.f}, .pivot = glm::vec2{1.f, 0.0f}});

  while (!done)
  {
    while (e.poll()) {}
    selector.setPos(
      glm::vec2{minX - 20.f, height / 16.f + 200.f + 100.f + 40.f + selection * selectionHeight});
    masterVolumeThumb.setPos(masterVolumePos + glm::vec2{200.f * masterVolume, 0.0f});
    musicThumb.setPos(musicPos + glm::vec2{200.f * music, 0.0f});
    sfxThumb.setPos(sfxPos + glm::vec2{200.f * sfx, 0.0f});
    fullScreenCheckMark->isVisible = fullScreen;
    showFpsCheckMark->isVisible = showFps;
    render.render(scene);
    bgfx::frame();
  }
  if (auto f = std::ofstream{prefPath + "settings.json", std::ios::binary})
    jsonSer(f, *this);
}
