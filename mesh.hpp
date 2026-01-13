#pragma once
#include "asset.hpp"
#include <string>

class Mesh final : public Asset
{
public:
  Mesh(std::string path);

private:
  std::string path;
};
