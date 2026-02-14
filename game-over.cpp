#include "game-over.hpp"
#include "consts.hpp"
#include "font.hpp"
#include "label-node.hpp"
#include "master-speaker.hpp"
#include "render.hpp"
#include "sample.hpp"
#include "sound-wave.hpp"
#include <sdlpp/sdlpp.hpp>

GameOver::GameOver(Assets &aAssets, Render &aRender, Sink &aSink, int aScore)
  : assets(aAssets), render(aRender), sink(aSink), scene(nullptr), score(aScore)
{
}

auto GameOver::run() -> void
{
  auto gameOverSound = Sample{sink, assets.get<SoundWave>("game-over"), 1., 0.0};
  gameOverSound.play();
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

  e.controllerButtonDown = [&](const auto &e) {
    switch (e.button)
    {
    case SDL_CONTROLLER_BUTTON_START:
    case SDL_CONTROLLER_BUTTON_A:
    case SDL_CONTROLLER_BUTTON_B: done = true; break;
    }
  };

  const auto width = render.getWidth();
  const auto height = render.getHeight();

  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "game over",
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 200});
    const auto dim = node.getDimensions(render);
    node.setPos(glm::vec2{(width - dim.x) / 2.f, (height / 2.f - dim.y) / 2.f});
  }
  {
    auto &node = scene.addNode<LabelNode>(Label{.text = "Your score: " + std::to_string(score),
                                                .font = assets.get<Font>("chp-fire.ttf"),
                                                .color = glm::vec3{1.f, 1.f, 1.f},
                                                .sz = 100});
    const auto dim = node.getDimensions(render);
    node.setPos(glm::vec2{(width - dim.x) / 2.f, height / 2});
  }
  while (!done)
  {
    while (e.poll()) {}
    render.render(scene);
    bgfx::frame();
  }
}
