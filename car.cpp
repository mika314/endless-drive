#include "car.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "spotlight.hpp"
#include <sdlpp/sdlpp.hpp>

Car::Car(BaseNode *parent, class Assets &assets)
  : VisualNodeRef<Mesh>(parent, assets.get<Mesh>("fancy-car.gltf/SM_vehCar_vehicle06_LOD"))
{
  {
    auto &tmpNode = addVisualNode<Mesh>(assets, "traffic-cone.gltf/SM_Cone01");
    tmpNode.setPos(glm::vec3{0.0f, -1.5f, 1.5f});
    tmpNode.setScale(glm::vec3{.5f});
  }
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
  const auto carYOffset = now / 1'000.f;
  setRot(glm::vec3{0.0f, 0.0f, now / 1000.f});
  setScale(glm::vec3{1.0f, 1.0f, 1.f + .1f * sin(now / 100.f)});
  setPos(glm::vec3{cos(now / 1000.f), sin(now / 1000.f) + carYOffset, 0.0f});
}
