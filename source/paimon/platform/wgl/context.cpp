#ifdef _WIN32

#include "paimon/platform/wgl/context.h"

#include <map>

#include "window.h"

#include "glad/wgl.h"

#include "paimon/core/base/macro.h"
#include "paimon/platform/context_format.h"
#include "paimon/platform/wgl/platform.h"

using namespace paimon;

bool WGLExtensionLoader::s_loaded = false;

void WGLExtensionLoader::Load() {
  if (s_loaded) {
    return;
  }

  HMODULE instance;
  if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   nullptr, &instance)) {
    LOG_ERROR("GetModuleHandleEx failed");
    return;
  }

  WNDCLASSEX windowClass = {
    .cbSize = sizeof(WNDCLASSEX),
    .style = CS_OWNDC,
    .lpfnWndProc = DefWindowProc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = instance,
    .hIcon = nullptr,
    .hCursor = nullptr,
    .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BACKGROUND),
    .lpszMenuName = nullptr,
    .lpszClassName = TEXT("WGLExtensionLoader"),
    .hIconSm = nullptr
  };

  auto id = RegisterClassEx(&windowClass);
  if (id == 0) {
    LOG_ERROR("RegisterClassEx failed");
    return;
  }

  auto hwnd =
      CreateWindow(reinterpret_cast<LPCTSTR>(id), nullptr, WS_OVERLAPPEDWINDOW,
                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                   nullptr, nullptr, instance, nullptr);

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
    .cRedBits = 8,
    .cRedShift = 0,
    .cGreenBits = 8,
    .cGreenShift = 0,
    .cBlueBits = 8,
    .cBlueShift = 0,
    .cAlphaBits = 8,
    .cAlphaShift = 0,
    .cAccumBits = 0,
    .cAccumRedBits = 0,
    .cAccumGreenBits = 0,
    .cAccumBlueBits = 0,
    .cAccumAlphaBits = 0,
    .cDepthBits = 24,
    .cStencilBits = 8,
    .cAuxBuffers = 0,
    .iLayerType = PFD_MAIN_PLANE,
    .bReserved = 0,
    .dwLayerMask = 0,
    .dwVisibleMask = 0,
    .dwDamageMask = 0
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

  if (gladLoaderLoadWGL(nullptr) == 0) {
    LOG_ERROR("Failed to load WGL extensions");
  }

  wglMakeCurrent(nullptr, nullptr);
  wglDeleteContext(dummyContext);
  ReleaseDC(hwnd, hdc);
  DestroyWindow(hwnd);
  UnregisterClass(windowClass.lpszClassName, instance);

  s_loaded = true;
}

std::vector<int> createContextAttributeList(const ContextFormat &format) {
  std::map<int, int> attributes;

  if (format.versionMajor > 0) {
    attributes[WGL_CONTEXT_MAJOR_VERSION_ARB] = format.versionMajor;
    attributes[WGL_CONTEXT_MINOR_VERSION_ARB] = format.versionMinor;
  }

  if (format.debug) {
    attributes[WGL_CONTEXT_FLAGS_ARB] = WGL_CONTEXT_DEBUG_BIT_ARB;
  }

  switch (format.profile) {
  case ContextProfile::Core:
    attributes[WGL_CONTEXT_PROFILE_MASK_ARB] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
    break;
  case ContextProfile::Compatibility:
    attributes[WGL_CONTEXT_PROFILE_MASK_ARB] =
        WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
    break;
  default:
    break;
  }

  std::vector<int> list;
  list.reserve(attributes.size() * 2 + 1);
  for (const auto &attribute : attributes) {
    list.push_back(attribute.first);
    list.push_back(attribute.second);
  }
  list.push_back(0); // finalize list

  return std::move(list);
}

WGLContext::WGLContext() : m_owning(true) {
}

WGLContext::~WGLContext() {}

bool WGLContext::destroy() {
  if (m_owning && m_contextHandle != nullptr) {
    auto currentContext = wglGetCurrentContext();
    if (currentContext == m_contextHandle) {
      doneCurrent();
    }

    auto success = wglDeleteContext(m_contextHandle);
    if (!success) {
      LOG_ERROR("wglDeleteContext failed");
      return false;
    }
    m_contextHandle = nullptr;
  }
  return true;
}

long long WGLContext::nativeHandle() {
  return reinterpret_cast<long long>(m_contextHandle);
}

bool WGLContext::valid() {
  return m_contextHandle != nullptr && m_window != nullptr;
}

bool WGLContext::makeCurrent() {
  auto success = wglMakeCurrent(m_window->hdc(), m_contextHandle);
  if (!success) {
    LOG_ERROR("wglMakeCurrent failed");
  }
  return success;
}

bool WGLContext::doneCurrent() {
  auto success = wglMakeCurrent(nullptr, nullptr);
  if (!success) {
    LOG_ERROR("wglMakeCurrent(nullptr, nullptr) failed");
  }
  return success;
}

std::unique_ptr<Context> WGLContext::getCurrent() {
  auto context = std::make_unique<WGLContext>();

  context->m_owning = false;

  context->m_contextHandle = wglGetCurrentContext();
  if (context->m_contextHandle == nullptr) {
    LOG_ERROR("wglGetCurrentContext failed");
    return nullptr;
  }

  auto deviceContext = wglGetCurrentDC();
  if (deviceContext == nullptr) {
    LOG_ERROR("wglGetCurrentDC failed");
    return nullptr;
  }

  auto window = WindowFromDC(deviceContext);
  if (window == nullptr) {
    LOG_ERROR("WindowFromDC failed");
    return nullptr;
  }

  context->m_window = std::make_unique<Window>(window, deviceContext);
  return context;
}

std::unique_ptr<Context> WGLContext::create(const ContextFormat &format) {
  auto context = std::make_unique<WGLContext>();

  WglPlatform::instance();

  context->m_window = std::make_unique<Window>();
  // load wgl extensions for the window's hdc
  context->setPixelFormat();
  context->createContext(nullptr, format);

  return context;
}

void WGLContext::setPixelFormat() const {
  const auto hdc = m_window->hdc();

  static const int attributes[] = {WGL_DRAW_TO_WINDOW_ARB,
                                   GL_TRUE,
                                   WGL_ACCELERATION_ARB,
                                   WGL_FULL_ACCELERATION_ARB,
                                   WGL_SUPPORT_OPENGL_ARB,
                                   GL_TRUE,
                                   0};

  int pixelFormatIndex;
  UINT numPixelFormats;
  auto success = wglChoosePixelFormatARB(hdc, attributes, nullptr, 1,
                                         &pixelFormatIndex, &numPixelFormats);
  if (!success) {
    LOG_ERROR("wglChoosePixelFormatARB failed");
  }
  if (numPixelFormats == 0) {
    LOG_ERROR("No suitable pixel format found");
  }

  PIXELFORMATDESCRIPTOR descriptor;
  success = DescribePixelFormat(hdc, pixelFormatIndex,
                                sizeof(PIXELFORMATDESCRIPTOR), &descriptor);
  if (!success) {
    LOG_ERROR("DescribePixelFormat failed");
  }

  success = SetPixelFormat(hdc, pixelFormatIndex, &descriptor);
  if (!success) {
    LOG_ERROR("SetPixelFormat failed");
  }
}

void WGLContext::createContext(HGLRC shared, const ContextFormat &format) {
  const auto contextAttributes = createContextAttributeList(format);

  m_contextHandle = wglCreateContextAttribsARB(
      m_window->hdc(), shared, contextAttributes.data());
  if (m_contextHandle == nullptr) {
    LOG_ERROR("wglCreateContextAttribsARB failed");
  }
}

#endif // _WIN32
