#pragma once
#include "base-asset.hpp"
#include "tex.hpp"
#include <functional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>

class Font final : public BaseAsset
{
public:
  Font(const std::string &path, class Assets &);
  Font(const Font &) = delete;
  Font(Font &&);

private:
  std::string path;
};

// TODO-Mika need to be implemented
