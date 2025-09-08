#pragma once

#ifdef __linux__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <fcntl.h>

namespace paimon {
class GlxPlatform {
public:
  static GlxPlatform* instance();

  Display *display() const;

private:
  GlxPlatform();

  ~GlxPlatform();

private:
  Display *m_display;
};

} // namespace paimon

#endif // __linux__