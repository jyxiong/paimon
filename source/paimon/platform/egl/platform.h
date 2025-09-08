#pragma once

#ifdef __linux__

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
  EGLDisplay m_display;
  int m_major;
  int m_minor;
};

} // namespace paimon

#endif // __linux__
