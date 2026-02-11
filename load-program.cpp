#include "load-program.hpp"
#include <bx/os.h>
#include <log/log.hpp>
#include <stdio.h>

static const bgfx::Memory *loadMem(const char *_filePath)
{
  FILE *fin = fopen(_filePath, "rb");

  if (fin == nullptr)
  {
    printf("%s not found\n", _filePath);
    fflush(stdout);
    return NULL;
  }

  long file_size;

  if (fseek(fin, 0, SEEK_END) != 0)
  {
    /* Handle error */
  }

  file_size = ftell(fin);
  if (file_size == -1)
  {
    /* Handle error */
  }

  uint32_t size = file_size;
  const bgfx::Memory *mem = bgfx::alloc(size + 1);

  rewind(fin);

  fread(mem->data, 1, size, fin);

  fclose(fin);

  mem->data[mem->size - 1] = '\0';

  return mem;
}

static bgfx::ShaderHandle loadShader(const char *_name)
{
  char filePath[512];

  const char *shaderPath = "data/";
  bx::strCopy(filePath, BX_COUNTOF(filePath), shaderPath);
  bx::strCat(filePath, BX_COUNTOF(filePath), _name);
  switch (bgfx::getRendererType())
  {
  case bgfx::RendererType::Noop:
  case bgfx::RendererType::Direct3D11:
  case bgfx::RendererType::Direct3D12: bx::strCat(filePath, BX_COUNTOF(filePath), ".hlsl"); break;
  case bgfx::RendererType::Agc:
  case bgfx::RendererType::Gnm: break;
  case bgfx::RendererType::Metal: break;
  case bgfx::RendererType::Nvn: break;
  case bgfx::RendererType::OpenGL: bx::strCat(filePath, BX_COUNTOF(filePath), ".glsl"); break;
  case bgfx::RendererType::OpenGLES: break;
  case bgfx::RendererType::Vulkan: bx::strCat(filePath, BX_COUNTOF(filePath), ".spv"); break;

  case bgfx::RendererType::Count: BX_ASSERT(false, "You should not be here!"); break;
  }

  bgfx::ShaderHandle handle = bgfx::createShader(loadMem(filePath));
  bgfx::setName(handle, _name);

  return handle;
}

auto loadProgram(const char *_vsName, const char *_fsName) -> bgfx::ProgramHandle
{
  // LOG("Loading shaders. Vertex shader:", _vsName, "fragment shader:", _fsName);
  bgfx::ShaderHandle vsh = loadShader(_vsName);
  bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
  if (NULL != _fsName)
  {
    fsh = loadShader(_fsName);
  }

  return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
}
