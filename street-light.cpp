#include "street-light.hpp"
#include "spotlight.hpp"

StreetLight::StreetLight(BaseNode *parent, Assets &assets)
  : MeshNode(parent, assets.get<Mesh>("street-light.gltf/SM_SingleLight_01"))
{
  auto &light = addNode3d<Spotlight>(
    2.f * (rand() % 10 != 0 ? glm::vec3{8.f} : glm::vec3{8.f, 7.f, 6.f}), 3.141592654f / 3.f);
  light.setPos({-0.009609f, -1.33846f, 4.25473f});
  light.setRot({-3.141592654f / 1.6f, 0.0f, 0.0f});
}
