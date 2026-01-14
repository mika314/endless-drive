#include "get-natives.hpp"
#include <bgfx/platform.h>
#include <bx/os.h>
#include <sdlpp/sdlpp.hpp>
BX_PRAGMA_DIAGNOSTIC_PUSH()
BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG("-Wextern-c-compat")
#include <SDL2/SDL_syswm.h>
BX_PRAGMA_DIAGNOSTIC_POP()

static void *sdlNativeWindowHandle(SDL_Window *_window)
{
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  if (!SDL_GetWindowWMInfo(_window, &wmi))
  {
    return NULL;
  }

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
  wl_egl_window *win_impl = (wl_egl_window *)SDL_GetWindowData(_window, "wl_egl_window");
  if (!win_impl)
  {
    int width, height;
    SDL_GetWindowSize(_window, &width, &height);
    struct wl_surface *surface = wmi.info.wl.surface;
    if (!surface)
      return nullptr;
    win_impl = wl_egl_window_create(surface, width, height);
    SDL_SetWindowData(_window, "wl_egl_window", win_impl);
  }
  return (void *)(uintptr_t)win_impl;
#else
  return (void *)wmi.info.x11.window;
#endif
#elif BX_PLATFORM_OSX || BX_PLATFORM_IOS
  return wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
  return wmi.info.win.window;
#elif BX_PLATFORM_ANDROID
  return wmi.info.android.window;
#endif // BX_PLATFORM_
}

void *getNativeWindowHandle(sdl::Window &w)
{
  return sdlNativeWindowHandle(w.get());
}

void *getNativeDisplayHandle(sdl::Window &w)
{
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  if (!SDL_GetWindowWMInfo(w.get(), &wmi))
  {
    return NULL;
  }

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
  return wmi.info.wl.display;
#else
  return wmi.info.x11.display;
#endif // ENTRY_CONFIG_USE_WAYLAND
#else
  return NULL;
#endif // BX_PLATFORM_*
}
