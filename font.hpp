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
  auto uiPass(class Render &,
              glm::vec3 color,
              glm::vec2 pos,
              float rot,
              float size,
              const std::string &text) -> void;

private:
  std::string path;
  std::unordered_map<int, std::reference_wrapper<Tex &>> atlas;
};
