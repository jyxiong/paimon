
#pragma once

#ifdef PAIMON_PLATFORM_EGL

#include <vector>

#include <glad/egl.h>

namespace paimon {
class EglPlatform {
public:
  static EglPlatform *instance();

public:
  EGLDisplay display() const;

private:
  EglPlatform();
  ~EglPlatform();

private:
  std::vector<EGLDeviceEXT> m_devices;
  EGLDisplay m_display;

  EGLint m_major_version;
  EGLint m_minor_version;
};

} // namespace paimon

#endif // PAIMON_PLATFORM_EGL
