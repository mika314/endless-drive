#pragma once
#include "node.hpp"

class Scene
{
public:
  template <typename T, typename... Args>
  auto addVisualNode(Args &&...args) -> VisualNode<T> &
  {
    return rootNode.addVisualNode<T>(std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  auto addVisualNode(class Assets &assets, Args &&...args) -> VisualNodeRef<T> &
  {
    return rootNode.addVisualNode<T>(assets, std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  auto addNode(Args &&...args) -> T &
  {
    return rootNode.addNode<T>(std::forward<Args>(args)...);
  }

  auto render(class Render &) const -> void;
  auto remove(BaseNode &) -> void;
  auto tick(float dt) -> void;

private:
  BaseNode rootNode = {nullptr};
};
