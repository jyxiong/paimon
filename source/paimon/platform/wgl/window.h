#pragma once

#ifdef _WIN32

#include <windows.h>

namespace paimon {

class Window {
public:
/**
 * @brief A singleton class that registers a window class for creating windows.
 * This class ensures that the window class is registered only once during the
 * application's lifetime.
 */
class WindowClass {
public:
  static WindowClass& instance();
  WindowClass(const WindowClass &) = delete;
  WindowClass(WindowClass &&other) = delete;
  ~WindowClass();

  LPCTSTR id() const;

  WindowClass &operator=(const WindowClass &) = delete;
  WindowClass &operator=(WindowClass &&other) = delete;

private:
  WindowClass();
  HMODULE m_instanceHandle;
  ATOM m_id;
};

public:
  Window();
  Window(HWND hwnd, HDC hdc);
  ~Window();

  HDC hdc() const;

private:
  HWND m_hwnd = nullptr;
  HDC m_hdc = nullptr;
  bool m_ownsHwnd = false;
};

} // namespace paimon

#endif // _WIN32