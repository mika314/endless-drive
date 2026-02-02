#pragma once
#include <ser/macro.hpp>

struct Envelope
{
  double attack = 0.01;                                     // sec
  double decay = 0.1;                                       // sec
  double sustain = 1.0;                                     // dB - on client, linear on the server
  double release = 0.1;                                     // sec
  auto amp(double sample, double duration) const -> double; // 0 - 1
  SER_PROPS(attack, decay, sustain, release);
};
