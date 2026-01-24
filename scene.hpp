#pragma once
#include "node.hpp"

class Scene
{
public:
  template <typename T, typename... Args>
  auto addNode3D(Args &&...args) -> Node3D<T> &
  {
    return rootNode.addNode3D<T>(std::forward<Args>(args)...);
  }

  template <typename T, typename... Args>
  auto addNode3D(class Assets &assets, Args &&...args) -> Node3DRef<T> &
  {
    return rootNode.addNode3D<T>(assets, std::forward<Args>(args)...);
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
