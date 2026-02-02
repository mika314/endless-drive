#include "assets.hpp"
#include "canister.hpp"
#include "car.hpp"
#include "coin.hpp"
#include "consts.hpp"
#include "font.hpp"
#include "get-natives.hpp"
#include "get-road-offset.hpp"
#include "img-node.hpp"
#include "label-node.hpp"
#include "live.hpp"
#include "mesh.hpp"
#include "render.hpp"
#include "scene.hpp"
#include "sound-wave.hpp"
#include "spotlight.hpp"
#include "street-light.hpp"
#include "tire.hpp"
#include <bgfx/platform.h>
#include <fstream>
#include <list>
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
      r.resolution.reset = 0; // BGFX_RESET_VSYNC;
      return r;
    }());
  }
  ~BgfxInit() { bgfx::shutdown(); }
};

auto main(int /*argc*/, char ** /*argv*/) -> int
{
  srand(time(nullptr));
  auto init = sdl::Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  const auto startLoadingTime = SDL_GetTicks();
  const auto width = 1920;
  const auto height = 1080;

  auto win = sdl::Window{"bgfx", 0, 0, width, height, SDL_WINDOW_SHOWN};

  auto bgfxInit = BgfxInit{win, width, height};
  bgfx::setDebug(BGFX_DEBUG_TEXT);

  auto score = 0;
  auto fuel = 100.f;
  auto lives = 3000000;

  auto render = Render{width, height};
  auto assets = Assets{};
  auto scene = Scene{nullptr};

  auto &car = scene.addNode<Car>(assets);

  std::list<std::reference_wrapper<BaseNode3d>> tmpNodes;
  std::list<std::reference_wrapper<Obstacle>> obstacles;

  auto want =
    SDL_AudioSpec{.freq = SampleRate, .format = AUDIO_F32LSB, .channels = ChN, .samples = 1024};
  auto have = SDL_AudioSpec{};
  auto audio = sdl::Audio{nullptr, false, &want, &have, 0};
  audio.pause(false);
  auto &coinSound = assets.get<SoundWave>("coin");

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
      if (rand() % 20 == 0)
      {
        auto &node = scene.addNode<Canister>(assets);
        const auto x = rand() % 3 - 1;
        node.setPos({dx + x * 2.6f, y, 0.0f});
        node.x = x;
        node.y = y;
        tmpNodes.push_back(node);
        obstacles.push_back(node);
      }
      else if (rand() % 100 == 0)
      {
        auto &node = scene.addNode<Live>(assets);
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
        audio.queue(coinSound.pcm.data(), coinSound.pcm.size() * sizeof(coinSound.pcm[0]));
      }
    }
  };

  for (auto i = -10; i < 100; ++i)
    addRoadMeshes(i);

  auto roadIdx = 100;

  auto &floor = scene.addNode3d<Mesh>(assets, "floor.gltf/Floor");

  auto done = false;
  auto e = sdl::EventHandler{};
  e.quit = [&done](const SDL_QuitEvent &) { done = true; };
  e.keyDown = [&](const SDL_KeyboardEvent &e) {
    switch (e.keysym.sym)
    {
    case SDLK_q: done = true; break;
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

  auto &scoreLb = scene.addNode<LabelNode>(Label{.text = "Score:",
                                                 .font = assets.get<Font>("chp-fire.ttf"),
                                                 .color = glm::vec3{0.f, 0.f, 1.f},
                                                 .sz = 100});
  scoreLb.setPos(glm::vec2{width - 500.f, 150.f});

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

  auto t0 = SDL_GetTicks();
  LOG("assets loading time:", t0 - startLoadingTime);
  auto cnt = 0;
  auto dt0 = t0;
  auto lastY = 0;

  while (!done)
  {
    while (e.poll()) {}
    const auto now = SDL_GetTicks();
    const auto carYOffset = Car::desiredY();
    floor.setPos({0.0f, carYOffset, 0.0f});

    const auto dt = (now - dt0) / 1000.f;
    dt0 = now;

    fuel -= (carYOffset - lastY) * 0.025f;
    lastY = carYOffset;
    if (fuel <= 0)
    {
      fuel = 100.f;
      --lives;
      if (!livesIco.empty())
      {
        auto &live = livesIco.back();
        scene.remove(live);
        livesIco.pop_back();
      }
    }
    for (auto &live : livesIco)
      live.get().setScale(glm::vec3{1.f + .1f * sinf(now * .02f)});

    if (lives <= 0)
      done = true;

    scene.tickInternal(dt);

    while (!obstacles.empty() && obstacles.front().get().y < carYOffset - 4.4f / 2.f)
      obstacles.pop_front();

    while (!obstacles.empty())
    {
      auto &obstacle = obstacles.front();
      if (obstacle.get().x == car.currentLane && std::abs(obstacle.get().y - carYOffset) < 4.4f / 2.f)
      {
        if (dynamic_cast<Canister *>(&obstacle.get()))
          fuel = std::min(100.f, fuel + 10.f);
        else if (dynamic_cast<Live *>(&obstacle.get()))
        {
          ++lives;
          auto &liveIco = livesIco
                            .emplace_back(scene.addNode<ImgNode>(Img{.tex = assets.get<Tex>("heart-ico"),
                                                                     .sz = glm::vec2{100.f, 100.f},
                                                                     .pivot = glm::vec2{.5f, 0.5f}}))
                            .get();
          liveIco.setPos(glm::vec2{width - 500.f + (livesIco.size() - 1) * 100.f, 100.f});
        }
        else if (dynamic_cast<Coin *>(&obstacle.get()))
          score += 10;
        else if (dynamic_cast<Tire *>(&obstacle.get()))
        {
          --lives;
          if (!livesIco.empty())
          {
            auto &live = livesIco.back();
            scene.remove(live);
            livesIco.pop_back();
          }
        }
        obstacle.get().onHit();
        obstacles.pop_front();
      }
      else
        break;
    }

    render.setCamPos({0.0f, carYOffset - 12.f, 2.3f});
    render.setCamRot({0.0f, 0.0f, 0.0f});
    render.render(scene);
    while (!tmpNodes.empty() && tmpNodes.front().get().getPos().y < carYOffset - 24.f)
    {
      scene.remove(tmpNodes.front());
      tmpNodes.pop_front();
    }
    for (; roadIdx * 2 - 200 < carYOffset; ++roadIdx)
      addRoadMeshes(roadIdx);

    scoreLb.text = "Score: " + std::to_string(score);
    fuelGaugeNiddleIco.setRot(2.f * fuel / 100.f - 1.f);

    bgfx::dbgTextClear();

    auto t1 = SDL_GetTicks();
    ++cnt;
    static float fps = 0.0f;
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
}
