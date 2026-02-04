#include "sample.hpp"
#include "consts.hpp"
#include "sink.hpp"
#include "sound-wave.hpp"
#include <cmath>
#include <log/log.hpp>

Sample::Sample(class Sink &sink, const SoundWave &soundWave, double aGain, double aPan)
  : Source(sink), sample(soundWave.pcm)
{
  gain = aGain;
  pan = aPan;
  isReady = true;
}

auto Sample::internalPull(int samples) -> std::vector<float>
{
  if (!isReady)
    return {};
  auto r = std::vector<float>{};
  r.reserve(samples * ChN);
  for (auto i = 0; i < ChN * samples; ++i)
  {
    const auto a = [this]() {
      auto a = 0.0f;
      for (const auto &n : notes)
      {
        const auto idx_ = pos - n.start;
        a += (idx_ >= 0 && idx_ < static_cast<int>(sample.get().size()))
               ? n.gain *
                   ((idx_ % 2 == 1) ? ((n.pan <= 0) ? 1.f : 1.f + n.pan)
                                    : ((n.pan <= 0) ? 1.f - n.pan : 1.f)) *
                   sample.get()[idx_]
               : 0.0f;
      }
      return a;
    }();
    ++pos;
    r.push_back(a);

    for (auto it = std::begin(notes); it != std::end(notes);)
    {
      if (static_cast<double>(pos - it->start) >= sample.get().size())
        it = notes.erase(it);
      else
        ++it;
    }
  }
  return r;
}

auto Sample::play(double gain, double pan) -> void
{
  sink.get().lock();
  notes.emplace_back(N{.gain = gain, .pan = pan, .start = pos});
  sink.get().unlock();
}

auto Sample::dur() const -> double
{
  return 1. * sample.get().size() / SampleRate;
}
