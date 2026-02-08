#pragma once
#include <functional>
#include <vector>

class Source
{
public:
  Source(class Sink &);
  virtual ~Source();
  auto pull(int samples) -> std::vector<float>;
  double gain = 1.0; // linear
  double pan = 0.0;

protected:
  std::reference_wrapper<Sink> sink;

  virtual auto internalPull(int samples) -> std::vector<float> = 0;
};
