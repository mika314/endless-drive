#pragma once
#include "scene.hpp"

class TitleScreen
{
public:
  enum class Opt { gameplay, settings, quit };
  TitleScreen(class Assets &, class Render &, class Sink &);
  auto run() -> Opt;

private:
  Assets &assets;
  Render &render;
  Sink &sink;
  Scene scene;
  Opt opt = Opt::gameplay;
};
