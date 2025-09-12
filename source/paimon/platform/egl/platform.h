#pragma once

#ifdef __linux__

#include <vector>

#include "glad/egl.h"

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
};

} // namespace paimon

#endif // __linux__
