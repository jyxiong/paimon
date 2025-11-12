#pragma once

#ifdef PAIMON_PLATFORM_X11

#include <X11/X.h>
#include <X11/Xlib.h>
#include <fcntl.h>

namespace paimon {
class GlxPlatform {
public:
  static GlxPlatform *instance();

  Display *display() const;

private:
  GlxPlatform();

  ~GlxPlatform();

private:
  Display *m_display;
};

} // namespace paimon

#endif // PAIMON_PLATFORM_X11