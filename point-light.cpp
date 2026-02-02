#include "point-light.hpp"
#include "render.hpp"

auto PointLight::lightPass(class Render &render, glm::mat4 trans) const -> void
{
  render(Render::PointLightIn{.trans = trans, .color = color});
}
