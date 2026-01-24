#include "node2d.hpp"
#include <glm/gtx/matrix_transform_2d.hpp>

auto BaseNode2d::getPos() const -> glm::vec2
{
  return pos;
}
auto BaseNode2d::getScale() const -> glm::vec2
{
  return scale;
}

auto BaseNode2d::getRot() const -> float
{
  return rot;
}

auto BaseNode2d::setPos(glm::vec2 v) -> void
{
  pos = v;
}

auto BaseNode2d::setScale(glm::vec2 v) -> void
{
  scale = v;
}

auto BaseNode2d::setRot(float v) -> void
{
  rot = v;
}

auto BaseNode2d::getTrans() const -> glm::mat3
{
  const auto p = dynamic_cast<const BaseNode2d *>(getParent());
  return glm::scale(glm::rotate(glm::translate(p ? p->getTrans() : glm::mat3(1.0f), pos), rot), scale);
}
