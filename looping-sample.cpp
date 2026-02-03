#include "looping-sample.hpp"
#include "consts.hpp"
#include "sink.hpp"
#include "sound-wave.hpp"
#include <cmath>
#include <log/log.hpp>

LoopingSample::LoopingSample(class Sink &sink, const SoundWave &soundWave, double aGain, double aPan)
  : Source(sink), sample(soundWave.pcm)
{
  gain = aGain;
  pan = aPan;
  isReady = true;
}

auto LoopingSample::internalPull(int samples) -> std::vector<float>
{
  if (!isReady)
    return {};
  auto r = std::vector<float>{};
  r.reserve(samples * ChN);
  for (auto i = 0; i < ChN * samples; ++i)
  {
    r.push_back(sample.get()[pos % sample.get().size()]);
    ++pos;
  }
  return r;
}
