#pragma once
#include "assets.hpp"
#include "node.hpp"
#include <functional>
#include <memory>
#include <vector>

class BaseNode;

class Scene
{
public:
  Scene(Assets &aAssets) : assets(aAssets) {}
  template <typename T, typename... Args>
  auto addVisualNode(Args &&...args) -> VisualNode<T> &
  {
    return static_cast<VisualNode<T> &>(*nodes.emplace_back(
      std::make_unique<VisualNode<T>>(assets.get().get<T>(std::forward<Args>(args)...))));
  }

  auto geomPass(class Render &) const -> void;
  auto lightPass(class Render &) const -> void;

  std::reference_wrapper<Assets> assets;
  std::vector<std::unique_ptr<BaseNode>> nodes;
};
