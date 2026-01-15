#include "mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <log/log.hpp>

Mesh::Mesh(const std::string &path)
{
  auto [lFilePath, lMeshName] = [&]() {
    auto idx = path.rfind('/');
    return std::pair{path.substr(0, idx), path.substr(idx + 1)};
  }();
  meshName = std::move(lMeshName);
  filePath = std::move(lFilePath);
  LOG(filePath, meshName);
  Assimp::Importer import;
  const aiScene *scene = import.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
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
  processNode(scene->mRootNode, scene);
  vbh = bgfx::createVertexBuffer(bgfx::makeRef(verts.data(), verts.size() * sizeof(verts[0])),
                                 Vert::msLayout);
  ibh = bgfx::createIndexBuffer(bgfx::makeRef(idxes.data(), idxes.size() * sizeof(idxes[0])));
  isInit = true;
}

Mesh::Mesh(Mesh &&mesh)
  : isInit(mesh.isInit),
    meshName(std::move(mesh.meshName)),
    filePath(std::move(mesh.filePath)),
    verts(std::move(mesh.verts)),
    idxes(std::move(mesh.idxes)),
    mat(std::move(mesh.mat)),
    vbh(mesh.vbh),
    ibh(mesh.ibh)
{
  mesh.isInit = false;
}

Mesh::~Mesh()
{
  if (!isInit)
    return;
  bgfx::destroy(ibh);
  bgfx::destroy(vbh);
}

auto Mesh::processNode(const aiNode *node, const aiScene *scene) -> void
{
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    auto mesh = scene->mMeshes[node->mMeshes[i]];
    if (mesh->mName.C_Str() != meshName)
    {
      LOG("Mesh:", mesh->mName.C_Str());
      continue;
    }
    LOG("Mesh is found:", meshName);
    processMesh(mesh, scene);
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++)
    processNode(node->mChildren[i], scene);
}

auto Mesh::processMesh(const aiMesh *mesh, const aiScene *scene) -> void
{
  for (auto i = 0U; i < mesh->mNumVertices; ++i)
  {
    verts.push_back(
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
      idxes.push_back(face.mIndices[j]);
  }

  // process material
  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    aiString name;
    material->Get(AI_MATKEY_NAME, name);
    LOG("Material:", name.C_Str());
    mat = filePath + "/" + name.C_Str();
  }
}

auto Mesh::arm() -> void
{
  bgfx::setIndexBuffer(ibh);
  bgfx::setVertexBuffer(0, vbh);
}
