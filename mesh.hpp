#pragma once
#include "base-asset.hpp"
#include "vert.hpp"
#include <assimp/scene.h>
#include <functional>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

class Mesh final : public BaseAsset
{
public:
  Mesh(const std::string &path, class Assets &);
  Mesh(const Mesh &) = delete;
  auto geomPass(class Render &, glm::mat4 trans) const -> void;
  auto lightPass(class Render &, glm::mat4 trans) const -> void;

private:
  std::string meshName;
  std::string filePath;

  struct Part
  {
    Part() = default;
    Part(const Part &) = delete;
    Part(Part &&);
    ~Part();

    std::vector<Vert> verts;
    std::vector<uint16_t> idxes;
    class Material *material = nullptr;
    bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
  };
  std::vector<Part> parts;

  auto processNode(Assets &, const aiNode *, const aiScene *) -> void;
  auto processMesh(Assets &, const aiMesh *, const aiScene *) -> void;
};
