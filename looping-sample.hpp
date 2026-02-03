#pragma once
#include "source.hpp"
#include <atomic>

class LoopingSample : public Source
{
public:
  LoopingSample(class Sink &, const class SoundWave &, double gain, double pan);

private:
  auto internalPull(int samples) -> std::vector<float> final;

  std::atomic_bool isReady = false;
  std::reference_wrapper<const std::vector<float>> sample;
  int pos = 0;
};
