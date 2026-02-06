#pragma once
#include "sink.hpp"
#include "source.hpp"
#include <deque>

class Send final : public Source, public Sink
{
public:
  Send(class Sink &);
  auto lock() const -> void final {}
  auto unlock() const -> void final {}

private:
  auto internalPull(int samples) -> std::vector<float> final;
};
