#include "scene.hpp"

auto Scene::render(class Render &render) const -> void
{
  rootNode.geomPass(render);
  rootNode.lightPass(render);
}

auto Scene::remove(BaseNode &n) -> void
{
  rootNode.remove(n);
}
