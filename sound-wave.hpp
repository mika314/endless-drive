#pragma once
#include "base-asset.hpp"
#include <cstdint>
#include <string>
#include <vector>

class SoundWave final : public BaseAsset
{
public:
  SoundWave(const std::string &path, class Assets &);

  std::vector<float> pcm;
};
