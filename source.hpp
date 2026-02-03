#pragma once
#include <deque>
#include <memory>
#include <vector>

class Source
{
public:
  Source(class Sink &);
  virtual ~Source();
  auto pull(int samples) -> std::vector<float>;
  auto send(class Sink &, double gain, double pan) -> class Send &;
  double gain = 0.0; // linear
  double pan = 0.0;

protected:
  std::reference_wrapper<Sink> sink;
  std::deque<std::vector<float>> q;
  std::vector<std::unique_ptr<Send>> sends;

  virtual auto internalPull(int samples) -> std::vector<float> = 0;
};
