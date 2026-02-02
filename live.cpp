#include "live.hpp"
#include <sdlpp/sdlpp.hpp>

Live::Live(BaseNode *parent, class Assets &assets) : Obstacle(parent, assets.get<Mesh>("live.gltf/Live"))
{
}

auto Live::tick(float dt) -> void
{
  const auto now = SDL_GetTicks();
  setRot({0.0f, 0.0f, now / 150.f});
  setScale(glm::vec3{1.f} * (1.f + .2f * sinf(now * .02f)));

  if (wasHit())
  {
    auto p = getPos();
    p += vel * dt;
    vel += glm::vec3(0.0f, -25.f, -9.8f) * dt;
    setPos(p);
  }
}

auto Live::onHit() -> void
{
  Obstacle::onHit();
  const auto now = 0.001f * SDL_GetTicks();
  vel = glm::vec3{0.0f, 30.f + 3.0e-4f * now * now, 7.5f};
}
