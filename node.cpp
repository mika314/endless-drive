#include "node.hpp"
#include <glm/ext/matrix_transform.hpp>

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

auto BaseVisualNode::getPos() const -> glm::vec3
{
  return pos;
}

auto BaseVisualNode::getScale() const -> glm::vec3
{
  return scale;
}

auto BaseVisualNode::getRot() const -> glm::vec3
{
  return rot;
}

auto BaseVisualNode::setPos(glm::vec3 v) -> void
{
  pos = v;
}

auto BaseVisualNode::setScale(glm::vec3 v) -> void
{
  scale = v;
}

auto BaseVisualNode::setRot(glm::vec3 v) -> void
{
  rot = v;
}

auto BaseVisualNode::getTrans() const -> glm::mat4
{
  auto p = dynamic_cast<const BaseVisualNode *>(getParent());
  auto r = p ? p->getTrans() : glm::mat4{1.0f};
  r = glm::translate(r, pos);

  r = glm::rotate(r, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
  r = glm::rotate(r, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
  r = glm::rotate(r, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));

  r = glm::scale(r, scale);
  return r;
}
