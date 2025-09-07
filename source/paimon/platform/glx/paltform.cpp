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

Platform::Platform() { 
  m_display = XOpenDisplay(nullptr);
  if (m_display == nullptr) {
      LOG_ERROR("Cannot open display");
  }

  int glx_version = gladLoaderLoadGLX(m_display, DefaultScreen(m_display));
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
