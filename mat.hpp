#pragma once
#include "base-asset.hpp"
#include <glm/vec4.hpp>
#include <string>
#include <variant>

class Mat final : public BaseAsset
{
public:
  Mat(const std::string &path, class Assets &, class aiMaterial *);

  std::variant<glm::vec4, class Tex *> baseColor;
  std::variant<float, class Tex *> metalic;
  std::variant<float, class Tex *> roughness;
};
