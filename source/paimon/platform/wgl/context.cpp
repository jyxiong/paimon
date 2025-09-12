#ifdef _WIN32

#include "paimon/platform/wgl/context.h"

#include <map>

#include "glad/wgl.h"

#include "paimon/core/base/macro.h"
#include "paimon/platform/context_format.h"
#include "paimon/platform/wgl/platform.h"

using namespace paimon;

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

WGLContext::~WGLContext() {
  destroy();
}

bool WGLContext::destroy() {
  if (m_owning && m_context != nullptr) {
    auto currentContext = wglGetCurrentContext();
    if (currentContext == m_context) {
      doneCurrent();
    }

    auto success = wglDeleteContext(m_context);
    if (!success) {
      LOG_ERROR("wglDeleteContext failed");
      return false;
    }
    m_context = nullptr;
  }
  return true;
}

long long WGLContext::nativeHandle() const {
  return reinterpret_cast<long long>(m_context);
}

bool WGLContext::valid() const {
  return m_context != nullptr && m_hdc != nullptr;
}

bool WGLContext::makeCurrent() const {
  auto success = wglMakeCurrent(m_hdc, m_context);
  if (!success) {
    LOG_ERROR("wglMakeCurrent failed");
  }
  return success;
}

bool WGLContext::doneCurrent() const {
  auto success = wglMakeCurrent(nullptr, nullptr);
  if (!success) {
    LOG_ERROR("wglMakeCurrent(nullptr, nullptr) failed");
  }
  return success;
}

std::unique_ptr<Context> WGLContext::getCurrent() {
  auto context = std::make_unique<WGLContext>();

  context->m_owning = false;

  context->m_hwnd = nullptr;

  context->m_context = wglGetCurrentContext();
  if (context->m_context == nullptr) {
    LOG_ERROR("wglGetCurrentContext failed");
    return nullptr;
  }

  context->m_hdc = wglGetCurrentDC();
  if (context->m_hdc == nullptr) {
    LOG_ERROR("wglGetCurrentDC failed");
    return nullptr;
  }

  return context;
}

std::unique_ptr<Context> WGLContext::create(const Context& shared, const ContextFormat &format) {
  if (shared.valid()) {
    LOG_ERROR("Shared context is not a WGLContext");
    return nullptr;
  }

  auto context = std::make_unique<WGLContext>();

  context->createWindow();
  context->setPixelFormat();
  context->createContext(
      reinterpret_cast<HGLRC>(shared.nativeHandle()), format);

  return context;
}

std::unique_ptr<Context> WGLContext::create(const ContextFormat &format) {
  auto context = std::make_unique<WGLContext>();

  context->createWindow();
  context->setPixelFormat();
  context->createContext(nullptr, format);

  return context;
}

void WGLContext::createWindow() {
  const auto &registrar = WGLPlatform::instance();

  m_hwnd = CreateWindowEx(0, reinterpret_cast<LPCTSTR>(registrar.getId()),
                          nullptr, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr,
                          nullptr, registrar.getModule(), nullptr);

  if (m_hwnd == nullptr) {
    LOG_ERROR("CreateWindow failed");
    return;
  }

  m_hdc = GetDC(m_hwnd);
  if (m_hdc == nullptr) {
    LOG_ERROR("GetDC failed");
    return;
  }
}

void WGLContext::setPixelFormat() const {

  const int attributes[] = {WGL_DRAW_TO_WINDOW_ARB,
                            GL_TRUE,
                            WGL_ACCELERATION_ARB,
                            WGL_FULL_ACCELERATION_ARB,
                            WGL_SUPPORT_OPENGL_ARB,
                            GL_TRUE,
                            0};

  int pixelFormatIndex;
  unsigned int numPixelFormats;
  auto success = wglChoosePixelFormatARB(m_hdc, attributes, nullptr, 1,
                                         &pixelFormatIndex, &numPixelFormats);
  if (!success) {
    LOG_ERROR("wglChoosePixelFormatARB failed");
  }
  if (numPixelFormats == 0) {
    LOG_ERROR("No suitable pixel format found");
  }

  PIXELFORMATDESCRIPTOR descriptor;
  success = DescribePixelFormat(m_hdc, pixelFormatIndex,
                                sizeof(PIXELFORMATDESCRIPTOR), &descriptor);
  if (!success) {
    LOG_ERROR("DescribePixelFormat failed");
  }

  success = SetPixelFormat(m_hdc, pixelFormatIndex, &descriptor);
  if (!success) {
    LOG_ERROR("SetPixelFormat failed");
  }
}

void WGLContext::createContext(HGLRC shared, const ContextFormat &format) {
  const auto contextAttributes = createContextAttributeList(format);

  m_context =
      wglCreateContextAttribsARB(m_hdc, shared, contextAttributes.data());
  if (m_context == nullptr) {
    LOG_ERROR("wglCreateContextAttribsARB failed");
  }
}

#endif // _WIN32
