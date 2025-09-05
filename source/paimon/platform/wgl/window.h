
#pragma once
#include <windows.h>
#include <memory>

namespace {
class WindowClass {
public:
  WindowClass();
  WindowClass(const WindowClass &) = delete;
  WindowClass(WindowClass &&other) = delete;
  ~WindowClass();

  LPCTSTR id() const;

  WindowClass &operator=(const WindowClass &) = delete;
  WindowClass &operator=(WindowClass &&other) = delete;

private:
  HMODULE m_instanceHandle;
  ATOM m_id;
};
} // namespace


namespace paimon {

class WindowsWindow {
public:
  WindowsWindow();
  WindowsWindow(HWND hwnd, HDC hdc);
  ~WindowsWindow();

private:
  HWND m_hwnd = nullptr;
  HDC m_hdc = nullptr;
  bool m_ownsHwnd = false;
  std::weak_ptr<WindowClass> m_windowClass; // 仅自己创建窗口时持有
};

} // namespace paimon