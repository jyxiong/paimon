#ifdef __linux__

#include "paimon/platform/glx/platform.h"

#include "glad/glx.h"

#include "paimon/core/base/macro.h"

using namespace paimon;

Platform *Platform::instance() {
  static Platform platform;
  return &platform;
}

Display *Platform::display() const { return m_display; }

int Platform::screen() const { return m_screen; }

Platform::Platform() { 
  
  m_display = XOpenDisplay(nullptr);
  if (m_display == nullptr) {
      LOG_ERROR("Cannot open display");
  }

  m_screen = DefaultScreen(m_display);

  int glx_version = gladLoaderLoadGLX(m_display, m_screen);
  if (!glx_version) {
      LOG_ERROR("Unable to load GLX.");
  }
}

Platform::~Platform() {
  if (m_display != nullptr) {
    XCloseDisplay(m_display);
    m_display = nullptr;
  }
}

#endif // __linux__
