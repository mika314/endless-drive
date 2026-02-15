#include "gameplay.hpp"
#include "canister.hpp"
#include "car.hpp"
#include "coin.hpp"
#include "consts.hpp"
#include "font.hpp"
#include "get-road-offset.hpp"
#include "img-node.hpp"
#include "label-node.hpp"
#include "live.hpp"
#include "looping-sample.hpp"
#include "mesh.hpp"
#include "multiplier.hpp"
#include "render.hpp"
#include "sample.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "sink.hpp"
#include "sound-wave.hpp"
#include "spotlight.hpp"
#include "street-light.hpp"
#include "tire.hpp"
#include <bgfx/platform.h>
#include <fstream>
#include <functional>
#include <list>
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>

Gameplay::Gameplay(Assets &aAssets,
                   Render &aRender,
                   Sink &aMusicSend,
                   Sink &aSfxSend,
                   Settings &aSettings)
  : assets(aAssets), render(aRender), musicSend(aMusicSend), sfxSend(aSfxSend), settings(aSettings)
{
}

auto Gameplay::run() -> int
{
  auto scene = Scene{nullptr};
  std::list<std::reference_wrapper<class BaseNode3d>> tmpNodes;
  std::list<std::reference_wrapper<class Obstacle>> obstacles;

  const auto startLoadingTime = SDL_GetTicks();

  auto &car = scene.addNode<Car>(assets);
  auto coinSound = Sample{sfxSend, assets.get<SoundWave>("coin"), .33, 0.0};
  auto liveSound = Sample{sfxSend, assets.get<SoundWave>("live"), .33, 0.0};
  auto tireHitSound = Sample{sfxSend, assets.get<SoundWave>("tire-hit"), 1., 0.0};
  auto fuelSound = Sample{sfxSend, assets.get<SoundWave>("fuel"), 1., 0.0};
  auto lowGasSound = Sample{sfxSend, assets.get<SoundWave>("low-gas"), 1., 0.0};
  auto emptyTankSound = Sample{sfxSend, assets.get<SoundWave>("empty-tank"), 1., 0.0};
  auto bgm = LoopingSample{musicSend, assets.get<SoundWave>("bgm"), .1, 0.0};
  auto multiplierSound = Sample{sfxSend, assets.get<SoundWave>("multiplier"), .33, 0.0};

  int lastTire = 50;
  int n = 1;
  auto addRoadMeshes = [&](const auto i) {
    const auto y = 2.f * i;
    const auto dx = getRoadOffset(y);
    {
      auto &node = scene.addNode3d<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
      node.setPos({dx + 4.f, y, 0.0f});
      node.setScale(glm::vec3{.5f});
      tmpNodes.push_back(node);
    }
    {
      auto &node = scene.addNode3d<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
      node.setPos({dx - 4.f, y, 0.0f});
      node.setScale(glm::vec3{.5f});
      tmpNodes.push_back(node);
    }
    if (i % 2 == 0)
    {
      {
        auto &node = scene.addNode3d<Mesh>(assets, "dash-lane.gltf/DashLane");
        node.setPos({dx + 1.33f, y, 0.0f});
        node.setRot({0.0f, 0.0f, -getRoadAngle(y)});
        tmpNodes.push_back(node);
      }
      {
        auto &node = scene.addNode3d<Mesh>(assets, "dash-lane.gltf/DashLane");
        node.setPos({dx - 1.33f, y, 0.0f});
        node.setRot({0.0f, 0.0f, -getRoadAngle(y)});
        tmpNodes.push_back(node);
      }
    }
    if (i % 5 == 0)
    {
      {
        auto &node = scene.addNode<StreetLight>(assets);
        node.setPos({dx + 5.f, y, 0.0f});
        node.setRot({0.0f, 0.0f, -3.141592654f / 2});
        tmpNodes.push_back(node);
      }
      {
        auto &node = scene.addNode<StreetLight>(assets);
        node.setPos({dx - 5.f, y, 0.0f});
        node.setRot({0.0f, 0.0f, 3.141592654f / 2});
        tmpNodes.push_back(node);
      }
    }
    if (i % 20 == 0)
    {
      {
        auto &node = scene.addNode3d<Mesh>(assets, "building.gltf/Building2");
        node.setPos({dx + 12.f, y, 0.0f});
        node.setRot({0.0f, 0.0f, -3.141592654f / 2 - getRoadAngle(y)});
        tmpNodes.push_back(node);
      }
      {
        auto &node = scene.addNode3d<Mesh>(assets, "building.gltf/Building2");
        node.setPos({dx - 12.f, y, 0.0f});
        node.setRot({0.0f, 0.0f, 3.141592654f / 2 - getRoadAngle(y)});
        tmpNodes.push_back(node);
      }
    }
    if (rand() % 10 == 0 && (i > lastTire + 5 || n < 1))
    {
      if (i <= lastTire + 5)
        ++n;
      else
        n = 0;
      lastTire = i;
      auto &node = scene.addNode<Tire>(assets);
      const auto x = rand() % 3 - 1;
      node.setPos({dx + x * 2.6f, y, 0.0f});
      node.x = x;
      node.y = y;
      tmpNodes.push_back(node);
      obstacles.push_back(node);
    }
    else if (rand() % 2 == 0)
    {
      if (rand() % 370 == 0)
      {
        auto &node = scene.addNode<Canister>(assets);
        const auto x = rand() % 3 - 1;
        node.setPos({dx + x * 2.6f, y, 0.0f});
        node.x = x;
        node.y = y;
        tmpNodes.push_back(node);
        obstacles.push_back(node);
      }
      else if (rand() % 800 == 0)
      {
        auto &node = scene.addNode<Live>(assets);
        const auto x = rand() % 3 - 1;
        node.setPos({dx + x * 2.6f, y, 0.0f});
        node.x = x;
        node.y = y;
        tmpNodes.push_back(node);
        obstacles.push_back(node);
      }
      else if (rand() % 100 == 0)
      {
        auto &node = scene.addNode<Multiplier>(assets);
        const auto x = rand() % 3 - 1;
        node.setPos({dx + x * 2.6f, y, 0.0f});
        node.x = x;
        node.y = y;
        tmpNodes.push_back(node);
        obstacles.push_back(node);
      }
      else
      {
        auto &node = scene.addNode<Coin>(assets);
        const auto x = rand() % 3 - 1;
        node.setPos({dx + x * 2.6f, y, 0.0f});
        node.x = x;
        node.y = y;
        tmpNodes.push_back(node);
        obstacles.push_back(node);
      }
    }
  };

  auto roadIdx = -10;

  auto &floor = scene.addNode3d<Mesh>(assets, "floor.gltf/Floor");

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
  e.keyDown = [&](const auto &e) {
    switch (e.keysym.sym)
    {
    case SDLK_ESCAPE: done = true; break;
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
  e.controllerButtonDown = [&](const auto &e) {
    switch (e.button)
    {
    case SDL_CONTROLLER_BUTTON_START:
    case SDL_CONTROLLER_BUTTON_B: done = true; break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
      if (car.currentLane > -1)
        --car.currentLane;
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
      if (car.currentLane < 1)
        ++car.currentLane;
      break;
    }
  };

  const auto width = render.getWidth();

  auto &scoreLb = scene.addNode<LabelNode>(Label{.text = "Score:",
                                                 .font = assets.get<Font>("chp-fire.ttf"),
                                                 .color = glm::vec3{1.f, 1.f, .5f},
                                                 .sz = 100});
  scoreLb.setPos(glm::vec2{width - 600.f, 150.f});
  auto &multiplierLb = scene.addNode<LabelNode>(Label{.text = "x1",
                                                      .font = assets.get<Font>("chp-fire.ttf"),
                                                      .color = glm::vec3{.5f, .5f, 1.f},
                                                      .sz = 100});
  multiplierLb.setPos(glm::vec2{width - 375.f, 350.f});

  auto livesIco = std::vector<std::reference_wrapper<ImgNode>>{};
  for (auto i = 0; i < std::min(lives, 10); ++i)
  {
    auto &liveIco = livesIco
                      .emplace_back(scene.addNode<ImgNode>(Img{.tex = assets.get<Tex>("heart-ico"),
                                                               .sz = glm::vec2{100.f, 100.f},
                                                               .pivot = glm::vec2{.5f, 0.5f}}))
                      .get();
    liveIco.setPos(glm::vec2{width - 500.f + i * 100.f, 100.f});
  }

  {
    auto &fuelGaugeIco = scene.addNode<ImgNode>(Img{.tex = assets.get<Tex>("fuel-gauge"),
                                                    .sz = glm::vec2{200.f, 100.f},
                                                    .pivot = glm::vec2{.5f, 1.f}});
    fuelGaugeIco.setPos(glm::vec2{width - 400.f, 350.f});
  }
  auto &fuelGaugeNiddleIco = scene.addNode<ImgNode>(Img{.tex = assets.get<Tex>("fuel-gauge-niddle"),
                                                        .sz = glm::vec2{25.f, 100.f},
                                                        .pivot = glm::vec2{.5f, 1.f}});
  fuelGaugeNiddleIco.setPos(glm::vec2{width - 400.f, 350.f});

  const auto gameStartTime = SDL_GetTicks();
  auto t0 = gameStartTime;
  LOG("assets loading time:", t0 - startLoadingTime);
  auto cnt = 0;
  auto dt0 = 0.0f;
  auto lastY = 0;

  while (!done)
  {
    while (e.poll()) {}
    const auto now = .001f * (SDL_GetTicks() - gameStartTime);
    const auto carYOffset = Car::desiredY(now);
    car.now = now;
    floor.setPos({0.0f, carYOffset, 0.0f});

    const auto dt = now - dt0;
    dt0 = now;

    const auto oldFuel = fuel;
    fuel -= (carYOffset - lastY) * .01875f;
    if (oldFuel > 20. && fuel <= 20.)
      lowGasSound.play();
    lastY = carYOffset;
    if (fuel <= 0)
    {
      emptyTankSound.play();
      fuel = 100.f;
      multiplier = 1;
      --lives;
      if (!livesIco.empty())
      {
        auto &live = livesIco.back();
        scene.remove(live);
        livesIco.pop_back();
      }
    }
    for (auto &live : livesIco)
      live.get().setScale(glm::vec3{1.f + .1f * sinf(now * 20.f)});

    if (lives <= 0)
      done = true;

    scene.tickInternal(dt);

    while (!obstacles.empty() && obstacles.front().get().y < carYOffset - 4.4f / 2.f)
      obstacles.pop_front();

    for (auto &obstacle : obstacles)
    {
      if (std::abs(obstacle.get().y - car.getPos().y) < 4.4f / 2.f)
      {
        if (obstacle.get().x != car.currentLane || obstacle.get().wasHit())
          continue;
        if (dynamic_cast<Canister *>(&obstacle.get()))
        {
          fuel = std::min(100.f, fuel + 33.f);
          fuelSound.play(1.0, .5 * car.currentLane);
        }
        else if (dynamic_cast<Live *>(&obstacle.get()))
        {
          liveSound.play(1.0, .5 * car.currentLane);
          ++lives;
          auto &liveIco = livesIco
                            .emplace_back(scene.addNode<ImgNode>(Img{.tex = assets.get<Tex>("heart-ico"),
                                                                     .sz = glm::vec2{100.f, 100.f},
                                                                     .pivot = glm::vec2{.5f, 0.5f}}))
                            .get();
          liveIco.setPos(glm::vec2{width - 500.f + (livesIco.size() - 1) * 100.f, 100.f});
        }
        else if (dynamic_cast<Multiplier *>(&obstacle.get()))
        {
          multiplierSound.play(1.0, .5 * car.currentLane);
          multiplier *= 2;
        }
        else if (dynamic_cast<Coin *>(&obstacle.get()))
        {
          coinSound.play(1.0, .5 * car.currentLane);
          score += multiplier;
        }
        else if (dynamic_cast<Tire *>(&obstacle.get()))
        {
          tireHitSound.play(1.0, .5 * car.currentLane);
          --lives;
          fuel = 100.f;
          multiplier = 1;
          if (!livesIco.empty())
          {
            auto &live = livesIco.back();
            scene.remove(live);
            livesIco.pop_back();
          }
        }
        obstacle.get().onHit(now);
      }
      else
        break;
    }
    for (; roadIdx * 2 - 200 < carYOffset; ++roadIdx)
      addRoadMeshes(roadIdx);

    render.setCamPos({0.0f, carYOffset - 12.f, 7.5f});
    render.setCamRot({-.25f, 0.0f, 0.0f});
    render.render(scene);
    while (!tmpNodes.empty() && tmpNodes.front().get().getPos().y < carYOffset - 24.f)
    {
      scene.remove(tmpNodes.front());
      tmpNodes.pop_front();
    }

    scoreLb.text = "Score: " + std::to_string(score);
    multiplierLb.text = "x" + std::to_string(multiplier);
    fuelGaugeNiddleIco.setRot(2.f * fuel / 100.f - 1.f);

    bgfx::dbgTextClear();

    auto t1 = SDL_GetTicks();
    ++cnt;
    static float fps = 0.0f;
    if (settings.showFps)
      bgfx::dbgTextPrintf(0, 4, 0x0f, "FPS: %f", fps);
    if (t1 - t0 >= 1'000)
    {
      fps = cnt * 1000. / (t1 - t0);
      cnt = 0;
      t0 = t1;
    }
    bgfx::frame();
  }
  LOG("Your score:", score);
  return score;
}
