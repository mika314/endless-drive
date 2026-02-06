#pragma once

class Gameplay
{
public:
  Gameplay(class Assets &, class Render &, class Sink &musicSend, Sink &sfxSend, class Settings &);
  auto run() -> int;

private:
  Assets &assets;
  Render &render;
  Sink &musicSend;
  Sink &sfxSend;
  Settings &settings;

  int score = 0;
  float fuel = 100.f;
  int lives = 3;
};
