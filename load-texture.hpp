#pragma once
#include <bgfx/bgfx.h>

auto loadTexture(const char *, uint64_t _flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE)
  -> bgfx::TextureHandle;
