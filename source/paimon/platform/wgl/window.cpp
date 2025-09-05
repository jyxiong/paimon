
#include "paimon/platform/wgl/window.h"
#include <memory>
#include "paimon/core/base/macro.h"

using namespace paimon;

namespace {
WindowClass::WindowClass() : m_id(0) {
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
  windowClass.lpszClassName = TEXT("playground");
  windowClass.hIconSm = nullptr;

  m_id = RegisterClassEx(&windowClass);
  if (m_id == 0) {
    LOG_ERROR("RegisterClassEx failed");
  }
}

WindowClass::~WindowClass() {
  if (m_id != 0) {
    UnregisterClass(id(), m_instanceHandle);
  }
}

LPCTSTR WindowClass::id() const {
  return reinterpret_cast<LPCTSTR>(m_id);
}

std::shared_ptr<WindowClass> GetWindowClassInstance() {
  static std::shared_ptr<WindowClass> instance = std::make_shared<WindowClass>();
  return instance;
}
} // namespace



WindowsWindow::WindowsWindow()
    : m_ownsHwnd(true), m_windowClass(GetWindowClassInstance()) {
  m_hdc = GetDC(m_hwnd);
}

WindowsWindow::WindowsWindow(HWND hwnd, HDC hdc)
    : m_hwnd(hwnd), m_hdc(hdc), m_ownsHwnd(false) {}

WindowsWindow::~WindowsWindow() {
  if (m_hdc && m_hwnd)
    ReleaseDC(m_hwnd, m_hdc);
}
