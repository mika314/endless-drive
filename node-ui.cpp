#include "node-ui.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/vec3.hpp>

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

auto BaseNodeUi::getTrans() const -> glm::mat4
{
  const auto p = dynamic_cast<const BaseNodeUi *>(getParent());
  return glm::scale(
    glm::rotate(glm::translate(p ? p->getTrans() : glm::mat4(1.0f), glm::vec3{pos, 0.0f}),
                rot,
                glm::vec3{0.0f, 0.0f, 1.f}),
    glm::vec3{scale, 1.f});
}
