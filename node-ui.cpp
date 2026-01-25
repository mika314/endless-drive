#include "node-ui.hpp"
#include <glm/gtx/matrix_transform_2d.hpp>

auto BaseNodeUi::getPos() const -> glm::vec2
{
  return pos;
}
auto BaseNodeUi::getScale() const -> glm::vec2
{
  return scale;
}

auto BaseNodeUi::getRot() const -> float
{
  return rot;
}

auto BaseNodeUi::setPos(glm::vec2 v) -> void
{
  pos = v;
}

auto BaseNodeUi::setScale(glm::vec2 v) -> void
{
  scale = v;
}

auto BaseNodeUi::setRot(float v) -> void
{
  rot = v;
}

auto BaseNodeUi::getTrans() const -> glm::mat3
{
  const auto p = dynamic_cast<const BaseNodeUi *>(getParent());
  return glm::scale(glm::rotate(glm::translate(p ? p->getTrans() : glm::mat3(1.0f), pos), rot), scale);
}
