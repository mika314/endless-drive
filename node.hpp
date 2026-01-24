#pragma once
#include "assets.hpp"
#include <memory>
#include <vector>

template <typename T>
class Node3dRef;

template <typename T>
class Node3d;

class Render;

class BaseNode
{
public:
  BaseNode(BaseNode *parent);
  BaseNode(const BaseNode &) = delete;
  auto geomPassInternal(Render &render) const -> void;
  auto getParent() -> BaseNode *;
  auto getParent() const -> const BaseNode *;
  auto lightPassInternal(Render &render) const -> void;
  auto remove(BaseNode &) -> void;
  auto tickInternal(float dt) -> void;
  virtual auto geomPass(Render & /*render*/) const -> void {}
  virtual auto lightPass(Render & /*render*/) const -> void {}
  virtual auto uiPass(Render & /*render*/) const -> void {}
  virtual auto tick(float /*dt*/) -> void {}
  virtual ~BaseNode() = default;

  template <typename T, typename... Args>
  auto addNode3d(Args &&...args) -> Node3d<T> &
  {
    return static_cast<Node3d<T> &>(
      *nodes.emplace_back(std::make_unique<Node3d<T>>(this, T{std::forward<Args>(args)...})));
  }

  template <typename T, typename... Args>
  auto addNode3d(Assets &assets, Args &&...args) -> Node3dRef<T> &
  {
    const auto &asset = assets.get<T>(std::forward<Args>(args)...);
    return static_cast<Node3dRef<T> &>(*nodes.emplace_back(std::make_unique<Node3dRef<T>>(this, asset)));
  }

  template <typename T, typename... Args>
  auto addNode(Args &&...args) -> T &
  {
    return static_cast<T &>(*nodes.emplace_back(std::make_unique<T>(this, std::forward<Args>(args)...)));
  }

private:
  std::vector<std::unique_ptr<BaseNode>> nodes;
  BaseNode *parent = nullptr;
};
