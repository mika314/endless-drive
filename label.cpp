#include "label.hpp"
#include "font.hpp"
#include "render.hpp"
#include <log/log.hpp>

auto Label::uiPass(class Render &render, glm::mat3 trans) const -> void
{
  const auto sizedFont = font.get().getSizedFont(size);
  render(Render::TextIn{.text = text, .font = sizedFont, .color = color, .trans = trans});
}
