#include "car.hpp"
#include "get-road-offset.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "spotlight.hpp"
#include <sdlpp/sdlpp.hpp>

Car::Car(BaseNode *parent, class Assets &assets)
  : MeshNode(parent, assets.get<Mesh>("fancy-car.gltf/SM_vehCar_vehicle06_LOD"))
{
  {
    auto &tmpNode = addVisualNode<Spotlight>(0.4f * glm::vec3{1.f}, .4f);
    tmpNode.setPos(glm::vec3{.596434f, 2.1f, .704822f});
  }
  {
    auto &tmpNode = addVisualNode<Spotlight>(0.4f * glm::vec3{1.f}, .4f);
    tmpNode.setPos(glm::vec3{-.596434f, 2.1f, .704822f});
  }
}

auto Car::tick(float) -> void
{
  const auto now = SDL_GetTicks();
  const auto carYOffset = desiredY();
  setRot({0.0f, 0.0f, 0.0f});
  setScale({1.0f, 1.0f, 1.f + .1f * sin(now / 100.f)});
  setPos({getRoadOffset(carYOffset), carYOffset, 0.0f});
}

auto Car::desiredY() -> float
{
  const auto now = SDL_GetTicks();
  return now / 125.f;
}
