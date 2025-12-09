#ifdef PAIMON_PLATFORM_WIN32

#include "paimon/platform/wgl_context/platform.h"

#include <glad/wgl.h>

#include "paimon/core/log_system.h"

using namespace paimon;

const TCHAR *WGLPlatform::s_name = TEXT("PaimonWindowClass");

WGLPlatform &WGLPlatform::instance() {
  static WGLPlatform instance;
  return instance;
}

WGLPlatform::~WGLPlatform() { UnregisterClass(s_name, m_module); }

WGLPlatform::WGLPlatform() {
  registerWindowClass();
  loadExtensions();
}

void WGLPlatform::registerWindowClass() {
  m_module = GetModuleHandle(0);
  if (m_module == nullptr) {
    LOG_ERROR("GetModuleHandleEx failed");
    return;
  }

  WNDCLASS windowClass = {
      .style = CS_OWNDC,
      .lpfnWndProc = DefWindowProc,
      .cbClsExtra = 0,
      .cbWndExtra = 0,
      .hInstance = m_module,
      .hIcon = 0,
      .hCursor = 0,
      .hbrBackground = 0,
      .lpszMenuName = 0,
      .lpszClassName = s_name,
  };

  m_id = RegisterClass(&windowClass);
  if (m_id == 0) {
    LOG_ERROR("RegisterClass failed");
    return;
  }
}

void WGLPlatform::loadExtensions() {
  auto hwnd = CreateWindow(reinterpret_cast<LPCTSTR>(m_id), nullptr,
                           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr,
                           m_module, nullptr);

  if (hwnd == nullptr) {
    LOG_ERROR("CreateWindow failed");
    return;
  }

  auto hdc = GetDC(hwnd);
  if (hdc == nullptr) {
    LOG_ERROR("GetDC failed");
    return;
  }

  PIXELFORMATDESCRIPTOR pixelFormatDesc = {
      .nSize = sizeof(PIXELFORMATDESCRIPTOR),
      .nVersion = 1,
      .dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW,
      .iPixelType = PFD_TYPE_RGBA,
      .cColorBits = 24,
      .cRedBits = 0,
      .cRedShift = 0,
      .cGreenBits = 0,
      .cGreenShift = 0,
      .cBlueBits = 0,
      .cBlueShift = 0,
      .cAlphaBits = 0,
      .cAlphaShift = 0,
      .cAccumBits = 0,
      .cAccumRedBits = 0,
      .cAccumGreenBits = 0,
      .cAccumBlueBits = 0,
      .cAccumAlphaBits = 0,
      .cDepthBits = 24,
      .cStencilBits = 0,
      .cAuxBuffers = 0,
      .iLayerType = PFD_MAIN_PLANE,
      .bReserved = 0,
      .dwLayerMask = 0,
      .dwVisibleMask = 0,
      .dwDamageMask = 0};

  auto pixelFormat = ChoosePixelFormat(hdc, &pixelFormatDesc);
  if (pixelFormat == 0) {
    LOG_ERROR("ChoosePixelFormat failed");
    return;
  }

  if (!SetPixelFormat(hdc, pixelFormat, &pixelFormatDesc)) {
    LOG_ERROR("SetPixelFormat failed");
    return;
  }

  auto dummyContext = wglCreateContext(hdc);
  if (dummyContext == nullptr) {
    LOG_ERROR("wglCreateContext failed");
    return;
  }

  if (!wglMakeCurrent(hdc, dummyContext)) {
    LOG_ERROR("wglMakeCurrent failed");
    return;
  }

  if (gladLoaderLoadWGL(hdc) == 0) {
    LOG_ERROR("Failed to load WGL extensions");
  }

  wglMakeCurrent(nullptr, nullptr);
  wglDeleteContext(dummyContext);
  ReleaseDC(hwnd, hdc);
  DestroyWindow(hwnd);
}

#endif // _WIN32