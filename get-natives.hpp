#pragma once

namespace sdl
{
  class Window;
}

void *getNativeWindowHandle(sdl::Window &);
void *getNativeDisplayHandle(sdl::Window &);
