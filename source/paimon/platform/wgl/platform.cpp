#ifdef _WIN32

#include "paimon/platform/wgl/platform.h"

#include "glad/wgl.h"

#include "paimon/core/base/macro.h"

using namespace paimon;


const TCHAR* WindowClassRegistrar::s_name = TEXT("PaimonWindowClass");

WindowClassRegistrar &WindowClassRegistrar::instance() {
  static WindowClassRegistrar instance;
  return instance;
}

WindowClassRegistrar::~WindowClassRegistrar() {
  UnregisterClass(s_name, m_module);
}

WindowClassRegistrar::WindowClassRegistrar() {
  auto success = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   nullptr, &m_module);
  if (!success) {
    LOG_ERROR("GetModuleHandleEx failed");
    return;
  }

  WNDCLASSEX windowClass = {
    .cbSize = sizeof(WNDCLASSEX),
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
    .hIconSm = 0
  };

  m_id = RegisterClassEx(&windowClass);
  if (m_id == 0) {
    LOG_ERROR("RegisterClassEx failed");
    return;
  }
}

WGLExtensionLoader &WGLExtensionLoader::instance() {
  static WGLExtensionLoader instance;
  return instance;
}

WGLExtensionLoader::WGLExtensionLoader() {
  const auto &registrar = WindowClassRegistrar::instance();

  auto hwnd =
      CreateWindow(reinterpret_cast<LPCTSTR>(registrar.getId()), nullptr, WS_OVERLAPPEDWINDOW,
                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                   nullptr, nullptr, registrar.getModule(), nullptr);

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
    .dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW, // 必须加上 PFD_DRAW_TO_WINDOW
    .iPixelType = PFD_TYPE_RGBA,
    // .cColorBits = 24,
    // .cRedBits = 0,
    // .cRedShift = 0,
    // .cGreenBits = 0,
    // .cGreenShift = 0,
    // .cBlueBits = 0,
    // .cBlueShift = 0,
    // .cAlphaBits = 0,
    // .cAlphaShift = 0,
    // .cAccumBits = 0,
    // .cAccumRedBits = 0,
    // .cAccumGreenBits = 0,
    // .cAccumBlueBits = 0,
    // .cAccumAlphaBits = 0,
    // .cDepthBits = 24,
    // .cStencilBits = 0,
    // .cAuxBuffers = 0,
    .iLayerType = PFD_MAIN_PLANE,
    // .bReserved = 0,
    // .dwLayerMask = 0,
    // .dwVisibleMask = 0,
    // .dwDamageMask = 0
  };

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

auto proc = wglGetProcAddress("wglChoosePixelFormatARB");
if (!proc) {
    // 说明驱动/环境/像素格式不支持
}

  if (gladLoaderLoadWGL(nullptr) == 0) {
    LOG_ERROR("Failed to load WGL extensions");
  }

  if (!wglChoosePixelFormatARB) {
    LOG_ERROR("wglChoosePixelFormatARB is nullptr!");
  }

  wglMakeCurrent(nullptr, nullptr);
  wglDeleteContext(dummyContext);
  ReleaseDC(hwnd, hdc);
  DestroyWindow(hwnd);
}

#endif // _WIN32