#include "canister.hpp"
#include <sdlpp/sdlpp.hpp>

Canister::Canister(BaseNode *parent, class Assets &assets)
  : Obstacle(parent, assets.get<Mesh>("canister.gltf/SM_Canister"))
{
}

auto Canister::tick(float dt) -> void
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

auto Canister::onHit() -> void
{
  Obstacle::onHit();
  const auto now = 0.001f * SDL_GetTicks();
  vel = glm::vec3{0.0f, 30.f + 3.0e-4f * now * now, 7.5f};
}
