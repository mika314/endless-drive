#pragma once
#include "node.hpp"
#include <functional>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <type_traits>

template <typename T, typename = void>
struct HasUiPass : std::false_type
{
};

template <typename T>
struct HasUiPass<T,
                 std::void_t<decltype(std::declval<const T>().uiPass(std::declval<class Render &>(),
                                                                     std::declval<glm::mat4>()))>>
  : std::true_type
{
};

class BaseNodeUi : public BaseNode
{
public:
  using BaseNode::BaseNode;
  auto getPos() const -> glm::vec2;
  auto getScale() const -> glm::vec2;
  auto getRot() const -> float;
  auto getTrans() const -> glm::mat4;
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
    if constexpr (HasUiPass<T>::value)
    {
      if (!isVisible)
        return;
      asset.get().uiPass(render, getTrans());
    }
    else
      BaseNodeUi::uiPass(render, getTrans());
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
    if constexpr (HasUiPass<T>::value)
    {
      if (!isVisible)
        return;
      T::uiPass(render, getTrans());
    }
    else
      BaseNodeUi::uiPass(render);
  }
};
