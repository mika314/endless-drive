#pragma once
#include "source.hpp"
#include <atomic>

class Sample : public Source
{
public:
  Sample(class Sink &, const class SoundWave &, double gain, double pan);
  auto play(double gain = 1., double pan = 0.0) -> void;
  auto dur() const -> double;

private:
  auto internalPull(int samples) -> std::vector<float> final;

  std::atomic_bool isReady = false;
  std::reference_wrapper<const std::vector<float>> sample;
  int pos = 0;
  struct N
  {
    double gain;
    double pan;
    int start;
  };
  std::vector<N> notes;
};
