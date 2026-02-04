#pragma once
#include "scene.hpp"
#include <list>

class Gameplay
{
public:
  Gameplay(class Assets &, class Render &, class Sink &);
  auto run() -> int;

private:
  Assets &assets;
  Render &render;
  Sink &sink;
  Scene scene;
  int score = 0;
  float fuel = 100.f;
  int lives = 3;
  std::list<std::reference_wrapper<class BaseNode3d>> tmpNodes;
  std::list<std::reference_wrapper<class Obstacle>> obstacles;
};
