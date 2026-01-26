#include "label.hpp"
#include "cube-atlas.hpp"
#include "font.hpp"
#include "render.hpp"
#include <log/log.hpp>

auto Label::uiPass(class Render &render, glm::mat4 trans) const -> void
{
  render(Render::TextIn{.text = text, .font = font, .sz = sz, .color = color, .trans = trans});
}
