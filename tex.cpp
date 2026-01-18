#include "tex.hpp"
#include "load-texture.hpp"
#include <bgfx/bgfx.h>
#include <log/log.hpp>

Tex::Tex(const std::string &path, class Assets &)
  : h(loadTexture(
      ("data/" + path).c_str())) // TODO-Mika Not sure if hardcoding the data directory is okay.
{
  LOG("Loading texture:", path);
  if (!isValid(h))
    LOG("Texture loading error");
}

Tex::~Tex()
{
  bgfx::destroy(h);
}

auto Tex::arm(int idx, bgfx::UniformHandle uni) const -> void
{
  bgfx::setTexture(idx, uni, h);
}
