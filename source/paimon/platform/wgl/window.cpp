
#ifdef _WIN32

#include "paimon/platform/wgl/window.h"

#include "paimon/core/base/macro.h"
#include <winuser.h>

using namespace paimon;

Window::WindowClass::WindowClass() : m_id(0) {
  auto success = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   nullptr, &m_instanceHandle);
  if (!success) {
    LOG_ERROR("GetModuleHandleEx failed");
  }

  WNDCLASSEX windowClass;
  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.style = CS_OWNDC;
  windowClass.lpfnWndProc = DefWindowProc;
  windowClass.cbClsExtra = 0;
  windowClass.cbWndExtra = 0;
  windowClass.hInstance = m_instanceHandle;
  windowClass.hIcon = nullptr;
  windowClass.hCursor = nullptr;
  windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BACKGROUND);
  windowClass.lpszMenuName = nullptr;
  windowClass.lpszClassName = TEXT("PaimonWindowClass");
  windowClass.hIconSm = nullptr;

  m_id = RegisterClassEx(&windowClass);
  if (m_id == 0) {
    LOG_ERROR("RegisterClassEx failed");
  }
}

Window::WindowClass::~WindowClass() {
  if (m_id != 0) {
    UnregisterClass(id(), m_instanceHandle);
  }
}

LPCTSTR Window::WindowClass::id() const { return reinterpret_cast<LPCTSTR>(m_id); }

Window::WindowClass &Window::WindowClass::instance() {
  static WindowClass instance;
  return instance;
}

Window::Window() : m_ownsHwnd(true) {
  HMODULE instance;
  auto success = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   nullptr, &instance);
  if (!success) {
    LOG_ERROR("GetModuleHandleEx failed");
  }

  m_hwnd =
      CreateWindow(WindowClass::instance().id(), nullptr, WS_OVERLAPPEDWINDOW,
                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                   nullptr, nullptr, instance, nullptr);

  if (!m_hwnd) {
    LOG_ERROR("CreateWindow failed");
  }

  m_hdc = GetDC(m_hwnd);
  if (!m_hdc) {
    LOG_ERROR("GetDC failed");
  }
}

Window::Window(HWND hwnd, HDC hdc)
    : m_hwnd(hwnd), m_hdc(hdc), m_ownsHwnd(false) {}

Window::~Window() {
  if (m_hdc && m_hwnd)
    ReleaseDC(m_hwnd, m_hdc);
}

HDC Window::hdc() const { return m_hdc; }

#endif // _WIN32
