#pragma once
#include <ser/macro.hpp>
#include <string>

namespace sdl
{
  class Window;
}

class Settings
{
public:
  Settings(class Assets &,
           sdl::Window &,
           class Render &,
           class Sink &master,
           Sink &musicSend,
           Sink &sfxSend);
  auto run() -> void;
  float masterVolume = 1.f;
  float music = 1.f;
  float sfx = 1.f;
  bool fullScreen = true;
  bool showFps = true;

  SER_PROPS(masterVolume, music, sfx, fullScreen, showFps);

private:
  Assets &assets;
  sdl::Window &win;
  Render &render;
  Sink &master;
  Sink &musicSend;
  Sink &sfxSend;
  std::string prefPath;
  int selection = 0;
};
