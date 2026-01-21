#include "canister.hpp"
#include <sdlpp/sdlpp.hpp>

Canister::Canister(BaseNode *parent, class Assets &assets)
  : MeshNode(parent, assets.get<Mesh>("canister.gltf/SM_Canister"))
{
}

auto Canister::tick(float) -> void
{
  const auto now = SDL_GetTicks();
  setRot({0.0f, 0.0f, now / 300.f});
}
