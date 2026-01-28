#include "tire.hpp"
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>

Tire::Tire(BaseNode *parent, class Assets &assets)
  : Obstacle(parent, assets.get<Mesh>("tires-bunch.gltf/SM_TiresBunch_02"))
{
}

auto Tire::tick(float dt) -> void
{
  const auto now = SDL_GetTicks();
  if (isHit)
  {
    setRot({0.0f, now / 500.f, now / 300.f});
    auto p = getPos();
    p += vel * dt;
    vel += glm::vec3(0.0f, -25.f, -9.8f) * dt;
    setPos(p);
  }
}

auto Tire::onHit() -> void
{
  isHit = true;
  const auto now = 0.001f * SDL_GetTicks();
  vel = glm::vec3{0.0f, 30.f + 3.0e-4f * now * now, 7.5f};
}
