#pragma once
#include "asset.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Assets
{
public:
  template <typename T>
  auto get(const std::string &path) -> T &
  {
    auto it = assets.find(path);
    if (it != std::end(assets))
      return static_cast<T &>(*it->second);
    auto tmp = assets.emplace(path, std::make_unique<T>(path));
    return static_cast<T &>(*tmp.first->second);
  }

private:
  std::unordered_map<std::string, std::unique_ptr<Asset>> assets;
};
