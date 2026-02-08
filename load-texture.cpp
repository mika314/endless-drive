#include "load-texture.hpp"
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <cstring>
#include <fstream>
#include <log/log.hpp>
#include <sdlpp/sdlpp.hpp>
#include <sstream>

static auto imageReleaseCb(void * /*ptr*/, void *userData) -> void
{
  bimg::ImageContainer *imageContainer = (bimg::ImageContainer *)userData;
  bimg::imageFree(imageContainer);
}

auto loadTexture(const char *fileName, uint64_t flags) -> bgfx::TextureHandle
{
  bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

  // const auto t0 = SDL_GetTicks();

  auto file = std::ifstream{fileName, std::ios::binary};
  if (!file)
    return BGFX_INVALID_HANDLE;
  auto content = std::vector<std::byte>{};
  content.resize(std::filesystem::file_size(fileName));
  file.read(reinterpret_cast<char *>(content.data()), content.size());

  static auto allocator = bx::DefaultAllocator{};

  auto imageContainer = bimg::imageParse(&allocator, content.data(), content.size());
  if (!imageContainer)
    return BGFX_INVALID_HANDLE;

  // const auto t1 = SDL_GetTicks();

  // LOG("image parsing time:", t1 - t0);

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

  // LOG("uploading texture to GPU:", SDL_GetTicks() - t1);

  return handle;
}
