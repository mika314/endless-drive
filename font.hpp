#pragma once
#include "base-asset.hpp"
#include "font-manager.hpp"
#include "tex.hpp"
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>

class Font final : public BaseAsset
{
public:
  Font(const std::string &path, class Assets &);
  Font(const Font &) = delete;
  Font(Font &&);
  ~Font();
  auto getSizedFont(int size) -> FontHandle;

private:
  std::string path;
  std::reference_wrapper<FontManager> fontManager;
  TrueTypeHandle fontFile;
  std::unordered_map<int, FontHandle> sizedFonts;
};
