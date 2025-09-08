#ifdef __linux__

#include "paimon/platform/egl/platform.h"

#include "paimon/core/base/macro.h"

using namespace paimon;

EglPlatform *EglPlatform::instance() {
  static EglPlatform inst;
  return &inst;
}

EglPlatform::EglPlatform() : m_display(nullptr) {
  int egl_version = gladLoaderLoadEGL(nullptr);
  if (!egl_version) {
    LOG_ERROR("Unable to load EGL.");
    return;
  }

  LOG_INFO("Loaded EGL {}.{} on first load.",
           GLAD_VERSION_MAJOR(egl_version), GLAD_VERSION_MINOR(egl_version));

  m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (m_display == EGL_NO_DISPLAY) {
    LOG_ERROR("Got no EGL display.");
    return;
  }

  if (!eglInitialize(m_display, nullptr, nullptr)) {
    LOG_ERROR("Unable to initialize EGL");
    m_display = nullptr;
    return;
  }

  egl_version = gladLoaderLoadEGL(m_display);
  if (!egl_version) {
    LOG_ERROR("Unable to reload EGL.");
    m_display = nullptr;
    return;
  }
  m_major = GLAD_VERSION_MAJOR(egl_version);
  m_minor = GLAD_VERSION_MINOR(egl_version);

  LOG_INFO("Loaded EGL {}.{} after reload.", m_major, m_minor);

  eglBindAPI(EGL_OPENGL_API);
}

EglPlatform::~EglPlatform() { eglTerminate(m_display); }

EGLDisplay EglPlatform::display() const { return m_display; }

#endif // __linux__