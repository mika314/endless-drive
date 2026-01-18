#pragma once
#include "tex.hpp"
#include <bgfx/bgfx.h>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace internal
{
  template <typename>
  inline constexpr bool always_false_v = false;

  template <typename T>
  constexpr auto getType() -> bgfx::UniformType::Enum
  {
    if constexpr (std::is_same_v<T, Tex>)
      return bgfx::UniformType::Sampler;
    else if constexpr (std::is_same_v<T, glm::vec4>)
      return bgfx::UniformType::Vec4;
    else if constexpr (std::is_same_v<T, glm::mat3>)
      return bgfx::UniformType::Mat3;
    else if constexpr (std::is_same_v<T, glm::mat4>)
      return bgfx::UniformType::Mat4;
    else
      static_assert(always_false_v<T>, "Unsupported uniform type");
  }
} // namespace internal

template <typename T>
class Uni
{
public:
  Uni(const char *name, T aV = {})
    : h(bgfx::createUniform(name, internal::getType<T>())), v(std::move(aV))
  {
  }
  Uni(const Uni &) = delete;
  ~Uni() { bgfx::destroy(h); }
  Uni &operator=(const T &aV)
  {
    v = std::move(aV);
    arm();
    return *this;
  }
  operator T() { return v; }
  auto get() -> T { return v; }
  auto arm() -> void { bgfx::setUniform(h, &v); }

private:
  bgfx::UniformHandle h;
  T v;
};

template <>
class Uni<Tex>
{
public:
  Uni(const char *name, int aIdx) : h(bgfx::createUniform(name, internal::getType<Tex>())), idx(aIdx) {}
  Uni(const Uni &) = delete;
  ~Uni() { bgfx::destroy(h); }
  Uni &operator=(const Tex &v)
  {
    v.arm(idx, h);
    return *this;
  }
  Uni &operator=(bgfx::TextureHandle aH)
  {
    bgfx::setTexture(idx, h, aH);
    return *this;
  }

private:
  bgfx::UniformHandle h;
  int idx;
};
