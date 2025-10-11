#ifdef __linux__

#include "paimon/platform/glx/platform.h"

#include "glad/glx.h"

#include "paimon/core/log/log_system.h"

using namespace paimon;

GlxPlatform *GlxPlatform::instance() {
  static GlxPlatform platform;
  return &platform;
}

Display *GlxPlatform::display() const { return m_display; }

GlxPlatform::GlxPlatform() { 
  m_display = XOpenDisplay(nullptr);
  if (m_display == nullptr) {
      LOG_ERROR("Cannot open display");
  }

  int glx_version = gladLoaderLoadGLX(m_display, DefaultScreen(m_display));
  if (!glx_version) {
      LOG_ERROR("Unable to load GLX.");
  }
}

GlxPlatform::~GlxPlatform() {
  if (m_display != nullptr) {
    XCloseDisplay(m_display);
    m_display = nullptr;
  }
}

#endif // __linux__
