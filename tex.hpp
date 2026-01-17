#pragma once
#include "base-asset.hpp"
#include <bgfx/bgfx.h>
#include <string>

class Tex final : public BaseAsset
{
public:
  Tex(const std::string &path);
  Tex(const Tex &) = delete;
  ~Tex();

// private: TODO-Mika make texture handle private
  bgfx::TextureHandle h;
};
