#pragma once
#include "node.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class BaseNode3d : public BaseNode
{
public:
  using BaseNode::BaseNode;
  auto getPos() const -> glm::vec3;
  auto getScale() const -> glm::vec3;
  auto getRot() const -> glm::vec3;
  auto setPos(glm::vec3) -> void;
  auto setScale(glm::vec3) -> void;
  auto setRot(glm::vec3) -> void;
  auto getTrans() const -> glm::mat4;
  bool isVisible = true;

private:
  glm::vec3 pos = {0.0f, 0.0f, 0.0f};
  glm::vec3 scale = {1.f, 1.f, 1.f};
  glm::vec3 rot = {0.0f, 0.0f, 0.0f};
};

template <typename T>
class Node3dRef : public BaseNode3d
{
public:
  Node3dRef(BaseNode *parent, const T &aAsset) : BaseNode3d(parent), asset(aAsset) {}
  auto geomPass(class Render &render) const -> void final
  {
    if (!isVisible)
      return;
    asset.get().geomPass(render, getTrans());
  }
  auto lightPass(class Render &render) const -> void final
  {
    if (!isVisible)
      return;
    asset.get().lightPass(render, getTrans());
  }

private:
  std::reference_wrapper<const T> asset;
};

template <typename T>
class Node3d : public BaseNode3d, public T
{
public:
  template <typename... Args>
  Node3d(BaseNode *parent, Args &&...args) : BaseNode3d(parent), T(std::forward<Args>(args)...)
  {
  }

  auto geomPass(class Render &render) const -> void final
  {
    if (!isVisible)
      return;
    T::geomPass(render, getTrans());
  }
  auto lightPass(class Render &render) const -> void final
  {
    if (!isVisible)
      return;
    T::lightPass(render, getTrans());
  }
};
