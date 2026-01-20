#include "street-light.hpp"
#include "spotlight.hpp"

StreetLight::StreetLight(BaseNode *parent, Assets &assets)
  : MeshNode(parent, assets.get<Mesh>("street-light.gltf/SM_SingleLight_01"))
{
  auto &light = addVisualNode<Spotlight>(glm::vec3{1.f}, 3.14f / 3.f);
  light.setPos({-0.009609f, -1.33846f, 4.25473f});
  light.setRot({-3.1415926f / 2.f, 0.0f, 0.0f});
}
