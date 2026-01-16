#pragma once
#include "base-asset.hpp"
#include "vert.hpp"
#include <assimp/scene.h>
#include <functional>
#include <string>
#include <vector>

class Mesh final : public BaseAsset
{
public:
  Mesh(const std::string &path, class Assets &);
  Mesh(const Mesh &) = delete;
  Mesh(Mesh &&);
  ~Mesh();
  auto arm() -> class Mat *;

private:
  bool isInit = false;
  std::string meshName;
  std::string filePath;
  std::vector<Vert> verts;
  std::vector<uint16_t> idxes;
  Mat *mat = nullptr;
  bgfx::VertexBufferHandle vbh;
  bgfx::IndexBufferHandle ibh;

  auto processNode(Assets &, const aiNode *, const aiScene *) -> void;
  auto processMesh(Assets &, const aiMesh *, const aiScene *) -> void;
};
