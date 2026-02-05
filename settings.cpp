#include "settings.hpp"
#include "consts.hpp"
#include "font.hpp"
#include "img-node.hpp"
#include "label-node.hpp"
#include "master-speaker.hpp"
#include "render.hpp"
#include "sample.hpp"
#include "sound-wave.hpp"
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>

Settings::Settings(Assets &aAssets, Render &aRender, Sink &aSink)
  : assets(aAssets), render(aRender), sink(aSink), scene(nullptr)
{
}

auto Settings::run() -> void
{
  auto coinSound = Sample{sink, assets.get<SoundWave>("coin"), 1., 0.0};
  auto done = false;
  auto e = sdl::EventHandler{};
  e.quit = [&done](const SDL_QuitEvent &) { done = true; };
  e.keyDown = [&](const SDL_KeyboardEvent &e) {
    switch (e.keysym.sym)
    {
    case SDLK_ESCAPE:
    case SDLK_SPACE:
    case SDLK_RETURN: done = true; break;
    }
  };

  auto y = height / 16.f;
  auto masterVolumePos = glm::vec2{};
  auto musicPos = glm::vec2{};
  auto sfxPos = glm::vec2{};
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
  }
  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Full Screen",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    node.setPos(glm::vec2{(width - dim.x) / 2.f, y});
    y += dim.y;
  }
  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Show FPS",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    node.setPos(glm::vec2{(width - dim.x) / 2.f, y});
    y += dim.y;
  }
  auto &masterVolumeThumb = scene.addNode<ImgNode>(
    Img{.tex = assets.get<Tex>("thumb"), .sz = glm::vec2{50.f, 50.f}, .pivot = glm::vec2{.5f, 0.0f}});
  masterVolumeThumb.setPos(masterVolumePos + glm::vec2{200.f, 0.0f});
  auto &musicThumb = scene.addNode<ImgNode>(
    Img{.tex = assets.get<Tex>("thumb"), .sz = glm::vec2{50.f, 50.f}, .pivot = glm::vec2{.5f, 0.0f}});
  musicThumb.setPos(musicPos + glm::vec2{200.f, 0.0f});
  auto &sfxThumb = scene.addNode<ImgNode>(
    Img{.tex = assets.get<Tex>("thumb"), .sz = glm::vec2{50.f, 50.f}, .pivot = glm::vec2{.5f, 0.0f}});
  sfxThumb.setPos(sfxPos + glm::vec2{200.f, 0.0f});

  while (!done)
  {
    while (e.poll()) {}
    render.render(scene);
    bgfx::frame();
  }
}
