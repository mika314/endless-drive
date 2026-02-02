#include "sink.hpp"
#include "source.hpp"
#include <cmath>

auto Sink::add(class Source *v) -> void
{
  lock();
  sources.insert(v);
  unlock();
}

auto Sink::del(Source *v) -> void
{
  lock();
  sources.erase(v);
  unlock();
}

auto Sink::mix(int samples) -> std::vector<float>
{
  auto r = std::vector<float>{};
  for (auto &s : sources)
  {
    if (!s->isBusy())
      continue;
    r.resize(samples * ChN);
    const auto chunk = s->pull(samples);
    const auto a = s->gain;
    const auto pan = static_cast<float>(s->pan);
    for (auto i = 0; i < std::min(samples * ChN, static_cast<int>(chunk.size())); ++i)
      r[i] +=
        a * ((i % 2 == 1) ? ((pan <= 0) ? 1.f : 1.f + pan) : ((pan <= 0) ? 1.f - pan : 1.f)) * chunk[i];
  }
  return r;
}
