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
  auto getSizedFont(int size) const -> FontHandle;
  auto getGlyphInfo(class Atlas &, FontHandle, CodePoint) const -> const GlyphInfo *;
  auto getFontInfo(FontHandle) const -> const FontInfo &;
  auto getKerning(FontHandle, CodePoint prev, CodePoint cur) const -> float;

private:
  std::string path;
  std::reference_wrapper<FontManager> fontManager;
  TrueTypeHandle fontFile;
  mutable std::unordered_map<int, FontHandle> sizedFonts;
};
