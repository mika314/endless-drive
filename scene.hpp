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
  template <typename T, typename... Args>
  auto addVisualNode(Args &&...args) -> VisualNode<T> &
  {
    return static_cast<VisualNode<T> &>(
      *nodes.emplace_back(std::make_unique<VisualNode<T>>(T{std::forward<Args>(args)...})));
  }

  template <typename T, typename... Args>
  auto addVisualNodeFromAssets(Assets &assets, Args &&...args) -> VisualNode<T> &
  {
    const auto &asset = assets.get<T>(std::forward<Args>(args)...);
    return static_cast<VisualNode<T> &>(*nodes.emplace_back(std::make_unique<VisualNodeRef<T>>(asset)));
  }

  auto render(class Render &) const -> void;

private:
  auto geomPass(class Render &) const -> void;
  auto lightPass(class Render &) const -> void;

  std::vector<std::unique_ptr<BaseNode>> nodes;
};
