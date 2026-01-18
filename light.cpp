#include "light.hpp"
#include "render.hpp"

auto Light::geomPass(class Render &) const -> void {}
auto Light::lightPass(class Render &render, glm::vec3 pos) const -> void
{
  render.setLightAndRender(pos, color);
}
