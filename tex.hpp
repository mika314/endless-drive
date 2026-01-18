#pragma once
#include "base-asset.hpp"
#include <bgfx/bgfx.h>
#include <string>

class Tex final : public BaseAsset
{
public:
  Tex(const std::string &path, class Assets &);
  Tex(const Tex &) = delete;
  ~Tex();
  auto arm(int idx, bgfx::UniformHandle) const -> void;

private:
  bgfx::TextureHandle h;
};
