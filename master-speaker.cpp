#include "master-speaker.hpp"
#include "consts.hpp"
#include "source.hpp"
#include <log/log.hpp>

MasterSpeaker::MasterSpeaker()
  : want{.freq = SampleRate, .format = AUDIO_F32LSB, .channels = ChN, .samples = 1024},
    audio(nullptr, false, &want, &have, 0, [this](Uint8 *stream, int len) {
      lock();
      const auto samples = len / sizeof(float) / ChN;
      const auto r = mix(samples);
      for (auto i = 0U; i < std::min(samples * ChN, r.size()); ++i)
        reinterpret_cast<float *>(stream)[i] = r[i];

      samplesProcessed += samples;

      for (auto i = r.size(); i < samples * ChN; ++i)
        reinterpret_cast<float *>(stream)[i] = 0.0f;

      unlock();
    })
{
  audio.pause(0);
}

auto MasterSpeaker::lock() const -> void
{
  const_cast<MasterSpeaker *>(this)->audio.lock();
}

auto MasterSpeaker::unlock() const -> void
{
  const_cast<MasterSpeaker *>(this)->audio.unlock();
}

auto MasterSpeaker::getSamplesProcessed() const -> int
{
  auto r = 0;
  lock();
  r = samplesProcessed;
  unlock();
  return r;
}
