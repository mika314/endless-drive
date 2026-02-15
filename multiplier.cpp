#include "multiplier.hpp"
#include <sdlpp/sdlpp.hpp>

Multiplier::Multiplier(BaseNode *parent, class Assets &assets)
  : Obstacle(parent, assets.get<Mesh>("multiplier.gltf/Multiplier"))
{
}

auto Multiplier::tick(float dt) -> void
{
  const auto now = SDL_GetTicks();
  setRot({0.0f, 0.0f, now / 300.f});

  if (wasHit())
  {
    auto p = getPos();
    p += vel * dt;
    vel += glm::vec3(0.0f, -25.f, -9.8f) * dt;
    setPos(p);
  }
}

auto Multiplier::onHit(float now) -> void
{
  Obstacle::onHit(now);
  vel = glm::vec3{0.0f, 30.f + 3.0e-4f * now * now, 11.25f};
}
