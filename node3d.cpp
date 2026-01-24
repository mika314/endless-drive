#include "node3d.hpp"
#include <glm/ext/matrix_transform.hpp>

auto BaseNode3D::getPos() const -> glm::vec3
{
  return pos;
}

auto BaseNode3D::getScale() const -> glm::vec3
{
  return scale;
}

auto BaseNode3D::getRot() const -> glm::vec3
{
  return rot;
}

auto BaseNode3D::setPos(glm::vec3 v) -> void
{
  pos = v;
}

auto BaseNode3D::setScale(glm::vec3 v) -> void
{
  scale = v;
}

auto BaseNode3D::setRot(glm::vec3 v) -> void
{
  rot = v;
}

auto BaseNode3D::getTrans() const -> glm::mat4
{
  auto p = dynamic_cast<const BaseNode3D *>(getParent());
  auto r = p ? p->getTrans() : glm::mat4{1.0f};
  r = glm::translate(r, pos);

  r = glm::rotate(r, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
  r = glm::rotate(r, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
  r = glm::rotate(r, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));

  r = glm::scale(r, scale);
  return r;
}
