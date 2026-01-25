#pragma once
#include "node.hpp"
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

class BaseNodeUi : public BaseNode
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
class NodeUiRef : public BaseNodeUi
{
public:
  NodeUiRef(BaseNode *parent, const T &aAsset) : BaseNodeUi(parent), asset(aAsset) {}
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
class NodeUi : public BaseNodeUi, public T
{
public:
  template <typename... Args>
  NodeUi(BaseNode *parent, Args &&...args) : BaseNodeUi(parent), T(std::forward<Args>(args)...)
  {
  }
  auto uiPass(class Render &render) const -> void final
  {
    if (!isVisible)
      return;
    T::uiPass(render, getTrans());
  }
};
