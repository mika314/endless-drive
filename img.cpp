#include "img.hpp"
#include "assets.hpp"
#include "render.hpp"
#include "tex.hpp"
#include <glm/ext/matrix_transform.hpp>

auto Img::uiPass(Render &render, glm::mat4 trans) const -> void
{
  const auto tmpTrans = glm::scale(trans, glm::vec3{sz, 1.f});
  bgfx::setTransform(&tmpTrans);
  render(Render::ImgIn{.tex = tex});
}
