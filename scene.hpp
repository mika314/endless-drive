#pragma once
#include "node.hpp"

class Scene
{
public:
  template <typename T, typename... Args>
  auto addNode3d(Args &&...args) -> Node3d<T> &
  {
    return rootNode.addNode3d<T>(std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  auto addNode3d(class Assets &assets, Args &&...args) -> Node3dRef<T> &
  {
    return rootNode.addNode3d<T>(assets, std::forward<Args>(args)...);
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
