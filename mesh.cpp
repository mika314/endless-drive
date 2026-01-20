#include "mesh.hpp"
#include "assets.hpp"
#include "material.hpp"
#include "render.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <log/log.hpp>

Mesh::Mesh(const std::string &path, Assets &assets)
{
  auto [lFilePath, lMeshName] = [&]() {
    auto idx = path.rfind('/');
    return std::pair{path.substr(0, idx), path.substr(idx + 1)};
  }();
  meshName = std::move(lMeshName);
  filePath = std::move(lFilePath);
  LOG(this, "Mesh", filePath, meshName);
  Assimp::Importer import;
  const aiScene *scene = import.ReadFile("data/" + filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    LOG("ERROR::ASSIMP::", import.GetErrorString());
    if (!scene)
      LOG("The scene is empty");
    else
    {
      if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        LOG("The scene is incomplete.");
      if (!scene->mRootNode)
        LOG("The scene does not have a root node.");
    }
    return;
  }
  processNode(assets, scene->mRootNode, scene);
}

Mesh::Part::Part(Part &&v)
  : verts(std::move(v.verts)), idxes(std::move(v.idxes)), material(v.material), vbh(v.vbh), ibh(v.ibh)
{
  v.vbh = BGFX_INVALID_HANDLE;
  v.ibh = BGFX_INVALID_HANDLE;
}

Mesh::Part::~Part()
{
  if (bgfx::isValid(ibh))
    bgfx::destroy(ibh);
  if (bgfx::isValid(vbh))
    bgfx::destroy(vbh);
}

auto Mesh::processNode(Assets &assets, const aiNode *node, const aiScene *scene) -> void
{
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    auto mesh = scene->mMeshes[node->mMeshes[i]];
    const auto aiMeshName = std::string{mesh->mName.C_Str()};
    // LOG("ai mesh name", aiMeshName);
    if (![&]() {
          if (aiMeshName.find(meshName) != 0)
            return false;
          if (aiMeshName.size() == meshName.size())
            return true;
          if (aiMeshName[meshName.size()] != '-')
            return false;
          for (auto i = meshName.size() + 1; i < aiMeshName.size(); ++i)
            if (!std::isdigit(aiMeshName[i]))
              return false;
          return true;
        }())
      continue;
    LOG(this, "Mesh is found:", aiMeshName);
    processMesh(assets, mesh, scene);
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++)
    processNode(assets, node->mChildren[i], scene);
}

auto Mesh::processMesh(Assets &assets, const aiMesh *mesh, const aiScene *scene) -> void
{
  auto &part = parts.emplace_back();
  for (auto i = 0U; i < mesh->mNumVertices; ++i)
  {
    part.verts.push_back(
      Vert{.pos = glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
           .norm = glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
           .uv = mesh->mTextureCoords[0]
                   ? glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}
                   : glm::vec2{}});
  }

  for (auto i = 0U; i < mesh->mNumFaces; ++i)
  {
    aiFace face = mesh->mFaces[i];
    for (auto j = 0U; j < face.mNumIndices; j++)
      part.idxes.push_back(face.mIndices[j]);
  }

  // process material
  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial *aiMat = scene->mMaterials[mesh->mMaterialIndex];
    aiString name;
    aiMat->Get(AI_MATKEY_NAME, name);
    LOG(this, "Material:", name.C_Str());
    part.material = &assets.get<Material>(filePath + "/" + name.C_Str(), aiMat);
  }
  part.vbh = bgfx::createVertexBuffer(
    bgfx::makeRef(part.verts.data(), part.verts.size() * sizeof(part.verts[0])), Vert::msLayout);
  part.ibh =
    bgfx::createIndexBuffer(bgfx::makeRef(part.idxes.data(), part.idxes.size() * sizeof(part.idxes[0])));
}

auto Mesh::geomPass(class Render &render, glm::mat4 trans) const -> void
{
  for (const auto &part : parts)
  {
    bgfx::setTransform(&trans);

    bgfx::setIndexBuffer(part.ibh);
    bgfx::setVertexBuffer(0, part.vbh);
    render.setMaterialAndRender(part.material);
  }
}

auto Mesh::lightPass(class Render &, glm::mat4) const -> void {}
