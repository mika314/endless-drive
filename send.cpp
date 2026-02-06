#include "send.hpp"
#include "consts.hpp"
#include <log/log.hpp>

Send::Send(Sink &sink) : Source(sink) {}

auto Send::internalPull(int samples) -> std::vector<float>
{
  return mix(samples);
}
