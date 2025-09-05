#pragma once

#ifdef _WIN32

#include <windows.h>

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

namespace paimon {

class WindowsWindow {
public:
  WindowsWindow();
  WindowsWindow(HWND hwnd, HDC hdc);
  ~WindowsWindow();

  HDC hdc() const;

private:
  HWND m_hwnd = nullptr;
  HDC m_hdc = nullptr;
  bool m_ownsHwnd = false;
};

} // namespace paimon

#endif // _WIN32