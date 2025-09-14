#ifdef __linux__

#include "paimon/platform/egl/platform.h"

#include "paimon/core/base/macro.h"

using namespace paimon;

EglPlatform *EglPlatform::instance() {
  static EglPlatform inst;
  return &inst;
}

EglPlatform::EglPlatform() : m_display(nullptr) {
  auto egl_version = gladLoaderLoadEGL(nullptr);
  if (!egl_version) {
    LOG_ERROR("Unable to load EGL.");
    return;
  }

  EGLint num_devices = 0;
  if (!eglQueryDevicesEXT(0, nullptr, &num_devices) || num_devices <= 0) {
    LOG_ERROR("No EGL devices found.");
    return;
  }
  m_devices.resize(num_devices);
  if (!eglQueryDevicesEXT(num_devices, m_devices.data(), &num_devices)){
    LOG_ERROR("Failed to query EGL devices.");
    m_devices.clear();
    return;
  }

  m_display = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT,
                                     m_devices[0], nullptr);
  if (m_display == EGL_NO_DISPLAY) {
    LOG_ERROR("Got no EGL display.");
    return;
  }

  if (!eglInitialize(m_display, &m_major_version, &m_minor_version)) {
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
}

EglPlatform::~EglPlatform() { eglTerminate(m_display); }

EGLDisplay EglPlatform::display() const { return m_display; }

#endif // __linux__