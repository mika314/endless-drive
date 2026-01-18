#include "node.hpp"
#include <glm/ext/matrix_transform.hpp>

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
  auto r = glm::mat4{1.0f};
  r = glm::translate(r, pos);

  r = glm::rotate(r, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
  r = glm::rotate(r, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
  r = glm::rotate(r, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));

  r = glm::scale(r, scale);
  return r;
}
