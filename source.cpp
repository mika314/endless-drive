#include "source.hpp"
#include "send.hpp"
#include "sink.hpp"
#include <cassert>
#include <log/log.hpp>

Source::Source(Sink &aSink) : sink(aSink)
{
  sink.get().add(this);
}

Source::~Source()
{
  sink.get().del(this);
}

auto Source::pull(int samples) -> std::vector<float>
{
  if (q.empty())
  {
    auto r = internalPull(samples);
    q.push_front(r);
    for (auto &s : sends)
      s->q.push_front(r);
  }
  auto r = std::move(q.back());
  q.pop_back();
  assert(r.empty() || static_cast<int>(r.size()) == ChN * samples);
  return r;
}

auto Source::send(class Sink &sink, double gain, double pan) -> class Send &
{
  return *sends.emplace_back(std::make_unique<Send>(sink, *this, gain, pan)).get();
}
