#include "title-screen.hpp"
#include "consts.hpp"
#include "font.hpp"
#include "img-node.hpp"
#include "label-node.hpp"
#include "render.hpp"
#include "sample.hpp"
#include "scene.hpp"
#include "sink.hpp"
#include "sound-wave.hpp"
#include <glm/vec2.hpp>
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>

TitleScreen::TitleScreen(Assets &aAssets, Render &aRender, Sink &aSink)
  : assets(aAssets), render(aRender), sink(aSink)
{
}

auto TitleScreen::run() -> Opt
{
  auto scene = Scene{nullptr};
  auto coinSound = Sample{sink, assets.get<SoundWave>("coin"), .33, 0.0};
  auto done = false;
  auto e = sdl::EventHandler{};
  e.quit = [&done, this](const SDL_QuitEvent &) {
    opt = Opt::quit;
    done = true;
  };
  e.windowEvent = [&](const SDL_WindowEvent &sdl_e) {
    if (sdl_e.event == SDL_WINDOWEVENT_RESIZED || sdl_e.event == SDL_WINDOWEVENT_SIZE_CHANGED)
    {
      const auto w = sdl_e.data1;
      const auto h = sdl_e.data2;
      bgfx::reset(w, h, ResetFlags);
      render.resize(w, h);
    }
  };
  e.keyDown = [&](const SDL_KeyboardEvent &e) {
    switch (e.keysym.sym)
    {
    case SDLK_UP:
      coinSound.play();
      opt = static_cast<Opt>((static_cast<int>(opt) + 3 - 1) % 3);
      break;
    case SDLK_DOWN:
      coinSound.play();
      opt = static_cast<Opt>((static_cast<int>(opt) + 1) % 3);
      break;
    case SDLK_SPACE:
    case SDLK_RETURN: done = true; break;
    }
  };

  const auto width = render.getWidth();
  const auto height = render.getHeight();

  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Play",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    node.setPos(glm::vec2{(width - dim.x) / 2.f, height / 2 - dim.y});
  }
  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Settings",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    node.setPos(glm::vec2{(width - dim.x) / 2.f, height / 2});
  }
  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Quit",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    node.setPos(glm::vec2{(width - dim.x) / 2.f, height / 2 + dim.y});
  }

  auto &selector = scene.addNode<ImgNode>(Img{
    .tex = assets.get<Tex>("selector"), .sz = glm::vec2{100.f, 100.f}, .pivot = glm::vec2{1.f, 0.0f}});
  while (!done)
  {
    while (e.poll()) {}
    selector.setPos(glm::vec2{(width - 500.f) / 2, height / 2 + 100. * (static_cast<int>(opt) - 1)});
    render.render(scene);
    bgfx::frame();
  }

  return opt;
}
