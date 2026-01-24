#pragma once
#include "node.hpp"

class BaseNode3D : public BaseNode
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
class Node3DRef : public BaseNode3D
{
public:
  Node3DRef(BaseNode *parent, const T &aAsset) : BaseNode3D(parent), asset(aAsset) {}
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
class Node3D : public BaseNode3D
{
public:
  Node3D(BaseNode *parent, const T &aAsset) : BaseNode3D(parent), asset(aAsset) {}
  auto geomPass(class Render &render) const -> void final
  {
    if (!isVisible)
      return;
    asset.geomPass(render, getTrans());
  }
  auto lightPass(class Render &render) const -> void final
  {
    if (!isVisible)
      return;
    asset.lightPass(render, getTrans());
  }

private:
  T asset;
};
