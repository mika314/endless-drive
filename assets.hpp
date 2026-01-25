#pragma once
#include "base-asset.hpp"
#include "font-manager.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class Assets
{
public:
  template <typename T, typename... Args>
  auto get(const std::string &path, Args &&...args) -> T &
  {
    auto it = assets.find(path);
    if (it != std::end(assets))
      return static_cast<T &>(*it->second);
    auto tmp = assets.emplace(path, std::make_unique<T>(path, *this, std::forward<Args>(args)...));
    return static_cast<T &>(*tmp.first->second);
  }

  FontManager fontManager;

private:
  std::unordered_map<std::string, std::unique_ptr<BaseAsset>> assets;
};
