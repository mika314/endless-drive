#pragma once
#include "assets.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>

template <typename T>
class VisualNodeRef;

template <typename T>
class VisualNode;

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
  auto addVisualNode(Args &&...args) -> VisualNode<T> &
  {
    return static_cast<VisualNode<T> &>(
      *nodes.emplace_back(std::make_unique<VisualNode<T>>(this, T{std::forward<Args>(args)...})));
  }

  template <typename T, typename... Args>
  auto addVisualNode(Assets &assets, Args &&...args) -> VisualNodeRef<T> &
  {
    const auto &asset = assets.get<T>(std::forward<Args>(args)...);
    return static_cast<VisualNodeRef<T> &>(
      *nodes.emplace_back(std::make_unique<VisualNodeRef<T>>(this, asset)));
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

class BaseVisualNode : public BaseNode
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

private:
  glm::vec3 pos = {0.0f, 0.0f, 0.0f};
  glm::vec3 scale = {1.f, 1.f, 1.f};
  glm::vec3 rot = {0.0f, 0.0f, 0.0f};
};

template <typename T>
class VisualNodeRef : public BaseVisualNode
{
public:
  VisualNodeRef(BaseNode *parent, const T &aAsset) : BaseVisualNode(parent), asset(aAsset) {}
  auto geomPass(class Render &render) const -> void final { asset.get().geomPass(render, getTrans()); }
  auto lightPass(class Render &render) const -> void final { asset.get().lightPass(render, getTrans()); }

private:
  std::reference_wrapper<const T> asset;
};

template <typename T>
class VisualNode : public BaseVisualNode
{
public:
  VisualNode(BaseNode *parent, const T &aAsset) : BaseVisualNode(parent), asset(aAsset) {}
  auto geomPass(class Render &render) const -> void final { asset.geomPass(render, getTrans()); }
  auto lightPass(class Render &render) const -> void final { asset.lightPass(render, getTrans()); }

private:
  T asset;
};
