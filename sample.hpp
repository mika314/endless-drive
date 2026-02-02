#pragma once
#include "envelope.hpp"
#include "note.hpp"
#include "source.hpp"
#include <atomic>

class Sample : public Source
{
public:
  Sample(const double &bpm, class Sink &, const class SoundWave &, double gain, double pan, Note);
  auto isBusy() const -> bool final;
  auto play(Note) -> void;
  auto set(Envelope) -> void;

private:
  auto internalPull(int samples) -> std::vector<float> final;

  std::atomic_bool isReady = false;
  std::reference_wrapper<const double> bpm;
  std::reference_wrapper<const std::vector<float>> sample;
  double sampleFreq;
  Envelope envelope;
  int pos = 0;
  struct N
  {
    double freq;
    double dur;
    double vel;

    int start;
  };
  std::vector<N> notes;
};
