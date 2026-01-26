#include "load-texture.hpp"
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <cstring>
#include <fstream>
#include <sstream>

static auto imageReleaseCb(void * /*ptr*/, void *userData) -> void
{
  bimg::ImageContainer *imageContainer = (bimg::ImageContainer *)userData;
  bimg::imageFree(imageContainer);
}

auto loadTexture(const char *fileName, uint64_t flags) -> bgfx::TextureHandle
{
  bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

  auto file = std::ifstream{fileName};
  if (!file)
    return BGFX_INVALID_HANDLE;

  auto buffer = std::stringstream{};
  buffer << file.rdbuf();
  const auto content = buffer.str();
  static auto allocator = bx::DefaultAllocator{};

  auto imageContainer = bimg::imageParse(&allocator, content.data(), content.size());
  if (!imageContainer)
    return BGFX_INVALID_HANDLE;

  const auto mem =
    bgfx::makeRef(imageContainer->m_data, imageContainer->m_size, imageReleaseCb, imageContainer);

  if (imageContainer->m_cubeMap)
    handle = bgfx::createTextureCube(uint16_t(imageContainer->m_width),
                                     1 < imageContainer->m_numMips,
                                     imageContainer->m_numLayers,
                                     bgfx::TextureFormat::Enum(imageContainer->m_format),
                                     flags,
                                     mem);
  else if (1 < imageContainer->m_depth)
    handle = bgfx::createTexture3D(uint16_t(imageContainer->m_width),
                                   uint16_t(imageContainer->m_height),
                                   uint16_t(imageContainer->m_depth),
                                   1 < imageContainer->m_numMips,
                                   bgfx::TextureFormat::Enum(imageContainer->m_format),
                                   flags,
                                   mem);
  else if (bgfx::isTextureValid(0,
                                false,
                                imageContainer->m_numLayers,
                                bgfx::TextureFormat::Enum(imageContainer->m_format),
                                flags))
    handle = bgfx::createTexture2D(uint16_t(imageContainer->m_width),
                                   uint16_t(imageContainer->m_height),
                                   1 < imageContainer->m_numMips,
                                   imageContainer->m_numLayers,
                                   bgfx::TextureFormat::Enum(imageContainer->m_format),
                                   flags,
                                   mem);

  if (bgfx::isValid(handle))
    bgfx::setName(handle, fileName, strlen(fileName));

  return handle;
}