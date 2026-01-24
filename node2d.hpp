#pragma once
#include "node.hpp"
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

class BaseNode2d : public BaseNode
{
public:
  using BaseNode::BaseNode;
  auto getPos() const -> glm::vec2;
  auto getScale() const -> glm::vec2;
  auto getRot() const -> float;
  auto getTrans() const -> glm::mat3;
  auto setPos(glm::vec2) -> void;
  auto setScale(glm::vec2) -> void;
  auto setRot(float) -> void;
  bool isVisible = true;

private:
  glm::vec2 pos = {0.0f, 0.0f};
  glm::vec2 scale = {1.f, 1.f};
  float rot = 0.0f;
};

template <typename T>
class Node2dRef : public BaseNode2d
{
public:
  Node2dRef(BaseNode *parent, const T &aAsset) : BaseNode2d(parent), asset(aAsset) {}
  auto uiPass(class Render &render) const -> void final
  {
    if (!isVisible)
      return;
    asset.get().geomPass(render, getTrans());
  }

private:
  std::reference_wrapper<const T> asset;
};

template <typename T>
class Node2d : public BaseNode2d
{
public:
  Node2d(BaseNode *parent, const T &aAsset) : BaseNode2d(parent), asset(aAsset) {}
  // TODO-Mika change the constructor so that would construct T in place
  auto uiPass(class Render &render) const -> void final
  {
    if (!isVisible)
      return;
    asset.uiPass(render, getTrans());
  }

private:
  T asset;
};
