#pragma once
#include "entity.hpp"
#include "sink.hpp"
#include <sdlpp/sdlpp.hpp>

class MasterSpeaker : public Sink
{
public:
  MasterSpeaker();
  auto lock() const -> void final;
  auto unlock() const -> void final;
  auto getSamplesProcessed() const -> int;
  std::vector<std::unique_ptr<Entity>> orphanage;

private:
  SDL_AudioSpec want;
  SDL_AudioSpec have;
  sdl::Audio audio;
  int samplesProcessed = 0;
};
