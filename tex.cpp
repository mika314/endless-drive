#include "tex.hpp"
#include "load-texture.hpp"
#include <bgfx/bgfx.h>
#include <log/log.hpp>

Tex::Tex(const std::string &path) : h(loadTexture(("data/" + path).c_str()))
{
  LOG("Loading texture:", path);
  if (!isValid(h))
    LOG("Texture loading error");
}

Tex::~Tex()
{
  bgfx::destroy(h);
}
