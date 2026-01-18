#include "scene.hpp"
#include <bgfx/bgfx.h>

auto Scene::geomPass(Render &render) const -> void
{
  for (const auto &node : nodes)
  {
    auto visualNode = dynamic_cast<const BaseVisualNode *>(node.get());
    if (!visualNode)
      continue;

    const auto trans = visualNode->getTrans();
    bgfx::setTransform(&trans);

    visualNode->geomPass(render);
  }
}

auto Scene::lightPass(Render &render) const -> void
{
  for (const auto &node : nodes)
  {
    auto visualNode = dynamic_cast<const BaseVisualNode *>(node.get());
    if (!visualNode)
      continue;
    visualNode->lightPass(render);
  }
}

auto Scene::render(class Render &render) const -> void
{
  geomPass(render);
  lightPass(render);
}
