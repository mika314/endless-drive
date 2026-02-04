#include "label.hpp"
#include "cube-atlas.hpp"
#include "font.hpp"
#include "render.hpp"
#include <log/log.hpp>

auto Label::uiPass(Render &render, glm::mat4 trans) const -> void
{
  render(Render::TextIn{.text = text, .font = font, .sz = sz, .color = color, .trans = trans});
}

auto Label::getDimensions(Render &render) const -> glm::vec2
{
  return render.getDimensions(font, sz, text);
}
