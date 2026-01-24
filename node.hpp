#pragma once
#include "assets.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>

template <typename T>
class Node3DRef;

template <typename T>
class Node3D;

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
  virtual auto tick(float /*dt*/) -> void {}
  virtual ~BaseNode() = default;

  template <typename T, typename... Args>
  auto addNode3D(Args &&...args) -> Node3D<T> &
  {
    return static_cast<Node3D<T> &>(
      *nodes.emplace_back(std::make_unique<Node3D<T>>(this, T{std::forward<Args>(args)...})));
  }

  template <typename T, typename... Args>
  auto addNode3D(Assets &assets, Args &&...args) -> Node3DRef<T> &
  {
    const auto &asset = assets.get<T>(std::forward<Args>(args)...);
    return static_cast<Node3DRef<T> &>(
      *nodes.emplace_back(std::make_unique<Node3DRef<T>>(this, asset)));
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

