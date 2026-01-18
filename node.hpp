#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class BaseNode
{
public:
  BaseNode() = default;
  BaseNode(const BaseNode &) = delete;
  virtual ~BaseNode() = default;
  virtual auto tick(float /*dt*/) -> void {}
};

class BaseVisualNode : public BaseNode
{
public:
  virtual auto geomPass(class Render &render) const -> void = 0;
  virtual auto lightPass(class Render &render) const -> void = 0;
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
class VisualNode final : public BaseVisualNode
{
public:
  VisualNode(T &aAsset) : asset(aAsset) {}
  auto geomPass(class Render &render) const -> void final { asset.get().geomPass(render); }
  auto lightPass(class Render &render) const -> void final { asset.get().lightPass(render); }

private:
  std::reference_wrapper<T> asset;
};
