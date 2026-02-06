#pragma once

class Settings
{
public:
  Settings(class Assets &, class Render &, class Sink &master, Sink &musicSend, Sink &sfxSend);
  auto run() -> void;
  float masterVolume = 1.f;
  float music = 1.f;
  float sfx = 1.f;
  bool fullScreen = true;
  bool showFps = true;

private:
  Assets &assets;
  Render &render;
  Sink &master;
  Sink &musicSend;
  Sink &sfxSend;
  int selection = 0;
};
