#include "node.hpp"
#include <algorithm>

BaseNode::BaseNode(BaseNode *aParent) : parent(aParent) {}

auto BaseNode::geomPassInternal(Render &render) const -> void
{
  for (const auto &node : nodes)
    node->geomPassInternal(render);
  geomPass(render);
}

auto BaseNode::lightPassInternal(Render &render) const -> void
{
  for (const auto &node : nodes)
    node->lightPassInternal(render);
  lightPass(render);
}

auto BaseNode::remove(BaseNode &n) -> void
{
  auto p = n.parent ? n.parent : this;
  auto it =
    std::find_if(std::begin(p->nodes), std::end(p->nodes), [&](const auto &x) { return x.get() == &n; });
  if (it == std::end(p->nodes))
    return;
  p->nodes.erase(it);
}

auto BaseNode::getParent() const -> const BaseNode *
{
  return parent;
}

auto BaseNode::getParent() -> BaseNode *
{
  return parent;
}

auto BaseNode::tickInternal(float dt) -> void
{
  for (const auto &node : nodes)
    node->tick(dt);
  tick(dt);
}

auto BaseNode::uiPassInternal(Render &render) const -> void
{
  for (const auto &node : nodes)
    node->uiPass(render);
  uiPass(render);
}
