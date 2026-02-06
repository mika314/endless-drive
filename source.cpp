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
  return internalPull(samples);
}
