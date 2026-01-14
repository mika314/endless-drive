#pragma once
#include "base-asset.hpp"
#include "vert.hpp"
#include <assimp/scene.h>
#include <string>
#include <vector>

class Mesh final : public BaseAsset
{
public:
  Mesh(const std::string &path);
  Mesh(const Mesh &) = delete;
  Mesh(Mesh &&);
  ~Mesh();
  auto arm() -> void;

private:
  bool isInit = false;
  std::string meshName;
  std::string filePath;
  std::vector<Vert> verts;
  std::vector<uint16_t> idxes;
  std::vector<std::string> mats;
  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;

  auto processNode(const aiNode *, const aiScene *) -> void;
  auto processMesh(const aiMesh *, const aiScene *) -> void;
};
