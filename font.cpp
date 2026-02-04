#include "font.hpp"
#include "assets.hpp"
#include "text-metrics.hpp"
#include <bgfx/bgfx.h>
#include <fstream>
#include <sstream>

static TrueTypeHandle loadTtf(FontManager &_fm, const std::string &_filePath)
{
  auto file = std::ifstream{_filePath};
  if (!file)
    return BGFX_INVALID_HANDLE;

  auto buffer = std::stringstream{};
  buffer << file.rdbuf();
  const auto content = buffer.str();

  auto size = content.size();
  auto data = content.data();

  if (NULL != data)
  {
    TrueTypeHandle handle = _fm.createTtf((uint8_t *)data, size);
    return handle;
  }

  TrueTypeHandle invalid = BGFX_INVALID_HANDLE;
  return invalid;
}

Font::Font(const std::string &aPath, class Assets &assets)
  : path(aPath), fontManager(assets.fontManager), fontFile(loadTtf(fontManager, "data/" + path))
{
}

Font::Font(Font &&v)
  : path(std::move(v.path)),
    fontManager(v.fontManager),
    fontFile(v.fontFile),
    sizedFonts(std::move(v.sizedFonts))
{
  v.fontFile.idx = bgfx::kInvalidHandle;
  v.sizedFonts = {};
}

Font::~Font()
{
  if (fontFile.idx != bgfx::kInvalidHandle)
    fontManager.get().destroyTtf(fontFile);
  for (auto &e : sizedFonts)
    fontManager.get().destroyFont(e.second);
}

auto Font::getSizedFont(float size) const -> FontHandle
{
  auto it = sizedFonts.find(size);
  if (it == std::end(sizedFonts))
  {
    auto tmp = sizedFonts.emplace(size, fontManager.get().createFontByPixelSize(fontFile, 0, size));
    assert(tmp.second);
    it = tmp.first;
  }
  return it->second;
}

auto Font::getGlyphInfo(Atlas &atlas, FontHandle h, CodePoint cp) const -> const GlyphInfo *
{
  return fontManager.get().getGlyphInfo(atlas, h, cp);
}

auto Font::getFontInfo(FontHandle h) const -> const FontInfo &
{
  return fontManager.get().getFontInfo(h);
}

auto Font::getKerning(FontHandle h, CodePoint prev, CodePoint cur) const -> float
{
  return fontManager.get().getKerning(h, prev, cur);
}

auto Font::getDimensions(Atlas &atlas, float size, const std::string &text) const -> glm::vec2
{
  auto metr = TextMetrics{&fontManager.get()};
  metr.appendText(atlas, getSizedFont(size), text.c_str());
  return glm::vec2{metr.getWidth(), metr.getHeight()};
}
