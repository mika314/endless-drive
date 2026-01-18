#pragma once
#include "base-asset.hpp"
#include "vert.hpp"
#include <assimp/scene.h>
#include <functional>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

class Mesh final : public BaseAsset
{
public:
  Mesh(const std::string &path, class Assets &);
  Mesh(const Mesh &) = delete;
  ~Mesh();
  auto geomPass(class Render &) const -> void;
  auto lightPass(class Render &, glm::vec3 pos) const -> void;

private:
  std::string meshName;
  std::string filePath;
  std::vector<Vert> verts;
  std::vector<uint16_t> idxes;
  class Material *material = nullptr;
  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;

  auto processNode(Assets &, const aiNode *, const aiScene *) -> void;
  auto processMesh(Assets &, const aiMesh *, const aiScene *) -> void;
};
