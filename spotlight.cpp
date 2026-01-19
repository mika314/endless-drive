#include "spotlight.hpp"
#include "render.hpp"

auto Spotlight::geomPass(class Render &, glm::mat4) const -> void {}
auto Spotlight::lightPass(class Render &render, glm::mat4 trans) const -> void
{
  render.setSpotlightAndRender(trans, color, angle);
}
