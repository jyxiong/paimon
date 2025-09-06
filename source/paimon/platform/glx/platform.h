#pragma once

#ifdef __linux__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <fcntl.h>

namespace paimon {
class Platform {
public:
  static Platform* instance();

  Display *display() const;

  int screen() const;

private:
  Platform();

  ~Platform();

private:
  Display *m_display;
  int m_screen;
};

} // namespace paimon

#endif // __linux__