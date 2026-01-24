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
    auto &tmpNode = addNode3D<Spotlight>(0.4f * glm::vec3{1.f}, .4f);
    tmpNode.setPos(glm::vec3{.596434f, 2.1f, .704822f});
  }
  {
    auto &tmpNode = addNode3D<Spotlight>(0.4f * glm::vec3{1.f}, .4f);
    tmpNode.setPos(glm::vec3{-.596434f, 2.1f, .704822f});
  }
}

auto Car::tick(float dt) -> void
{
  const auto now = SDL_GetTicks();
  const auto carYOffset = desiredY();
  setScale({1.0f, 1.0f, 1.f + .03f * sin(now / 100.f)});
  auto laneOffset = 0.f;
  laneOffset = 2.6f * currentLane;
  const auto desiredX = getRoadOffset(carYOffset) + laneOffset;
  const auto curX = getPos().x;
  const auto dx = 10.f * dt * (desiredX - curX);
  setPos({curX + dx, carYOffset, 0.0f});
  setRot({0.f, 0.f, -getRoadAngle(carYOffset) - 2.f * dx});
}

auto Car::desiredY() -> float
{
  const auto now = SDL_GetTicks();
  return (now + 1e-11f * now * now * now) * 0.015f;
}
