#pragma once
#include "base-asset.hpp"
#include <glm/vec4.hpp>
#include <string>
#include <variant>

class Material final : public BaseAsset
{
public:
  Material(const std::string &path, class Assets &, class aiMaterial *);

  std::variant<glm::vec4, class Tex *> baseColor;
  std::variant<float, class Tex *> metallic;
  std::variant<float, class Tex *> roughness;
  std::variant<glm::vec4, class Tex *> emission;
};
