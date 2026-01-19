#include "point-light.hpp"
#include "render.hpp"

auto PointLight::geomPass(class Render &, glm::mat4) const -> void {}
auto PointLight::lightPass(class Render &render, glm::mat4 trans) const -> void
{
  render.setPointLightAndRender(glm::vec3{trans[3]}, color);
}
