#pragma once
#include "scene.hpp"

class Settings
{
public:
  Settings(class Assets &, class Render &, class Sink &);
  auto run() -> void;

private:
  Assets &assets;
  Render &render;
  Sink &sink;
  Scene scene;
  int score;
};
