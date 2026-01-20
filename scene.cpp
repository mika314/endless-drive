#include "scene.hpp"

auto Scene::render(class Render &render) const -> void
{
  rootNode.geomPassInternal(render);
  rootNode.lightPassInternal(render);
}

auto Scene::remove(BaseNode &n) -> void
{
  rootNode.remove(n);
}

auto Scene::tick(float dt) -> void
{
  rootNode.tickInternal(dt);
}
