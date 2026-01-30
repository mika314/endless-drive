#include "material.hpp"
#include "assets.hpp"
#include "tex.hpp"
#include <assimp/material.h>
#include <filesystem>
#include <log/log.hpp>

static auto png2dds(const aiString &v) -> std::string
{
  auto p = std::filesystem::path{v.C_Str()};
  return p.stem().string();
}

Material::Material(const std::string & /*path*/, class Assets &assets, class aiMaterial *aiMat)
{
  // LOG("Loading material:", path);

  if (aiMat->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
  {
    auto str = aiString{};
    aiMat->GetTexture(aiTextureType_BASE_COLOR, 0, &str);
    baseColor = &assets.get<Tex>(png2dds(str));
  }
  else
  {
    auto color = aiColor4D{1.0f, 1.0f, 1.0f, 1.0f};
    aiMat->Get(AI_MATKEY_BASE_COLOR, color);
    baseColor = glm::vec4(color.r, color.g, color.b, color.a);
    // LOG("base color:", color.r, color.g, color.b, color.a);
  }

  if (aiMat->GetTextureCount(aiTextureType_METALNESS) > 0)
  {
    auto str = aiString{};
    aiMat->GetTexture(aiTextureType_METALNESS, 0, &str);
    metallic = &assets.get<Tex>(png2dds(str));
  }
  else if (aiMat->GetTextureCount(aiTextureType_UNKNOWN) > 0)
  {
    auto str = aiString{};
    aiMat->GetTexture(aiTextureType_UNKNOWN, 0, &str);
    metallic = &assets.get<Tex>(png2dds(str));
  }
  else
  {
    auto m = 0.0f;
    aiMat->Get(AI_MATKEY_METALLIC_FACTOR, m);
    metallic = m;
    // LOG("metallic:", m);
  }

  if (aiMat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
  {
    auto str = aiString{};
    aiMat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &str);
    roughness = &assets.get<Tex>(png2dds(str));
  }
  else if (aiMat->GetTextureCount(aiTextureType_UNKNOWN) > 0)
  {
    auto str = aiString{};
    aiMat->GetTexture(aiTextureType_UNKNOWN, 0, &str);
    roughness = &assets.get<Tex>(png2dds(str));
  }
  else
  {
    auto r = 0.5f;
    aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, r);
    roughness = r;
    // LOG("roughness:", r);
  }

  if (aiMat->GetTextureCount(aiTextureType_EMISSIVE) > 0)
  {
    auto str = aiString{};
    aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &str);
    emission = &assets.get<Tex>(png2dds(str));
  }
  else
  {
    auto color = aiColor4D{0.0f, 0.0f, 0.0f, 0.0f};
    aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
    emission = glm::vec4(color.r, color.g, color.b, color.a);
    // LOG("emission:", color.r, color.g, color.b, color.a);
  }
}
