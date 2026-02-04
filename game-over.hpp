#pragma once
#include "scene.hpp"

class GameOver
{
public:
  GameOver(class Assets &, class Render &, class Sink &, int score);
  auto run() -> void;

private:
  Assets &assets;
  Render &render;
  Sink &sink;
  Scene scene;
  int score;
};
