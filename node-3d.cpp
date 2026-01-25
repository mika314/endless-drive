#include "node-3d.hpp"
#include <glm/ext/matrix_transform.hpp>

auto BaseNode3d::getPos() const -> glm::vec3
{
  return pos;
}

auto BaseNode3d::getScale() const -> glm::vec3
{
  return scale;
}

auto BaseNode3d::getRot() const -> glm::vec3
{
  return rot;
}

auto BaseNode3d::setPos(glm::vec3 v) -> void
{
  pos = v;
}

auto BaseNode3d::setScale(glm::vec3 v) -> void
{
  scale = v;
}

auto BaseNode3d::setRot(glm::vec3 v) -> void
{
  rot = v;
}

auto BaseNode3d::getTrans() const -> glm::mat4
{
  const auto p = dynamic_cast<const BaseNode3d *>(getParent());
  return glm::scale(
    glm::rotate(glm::rotate(glm::rotate(glm::translate(p ? p->getTrans() : glm::mat4{1.0f}, pos),
                                        rot.z,
                                        glm::vec3(0.0f, 0.0f, 1.0f)),
                            rot.x,
                            glm::vec3(1.0f, 0.0f, 0.0f)),
                rot.y,
                glm::vec3(0.0f, 1.0f, 0.0f)),
    scale);
}
