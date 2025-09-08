#ifdef _WIN32

#include "paimon/platform/wgl/context.h"

#include <map>

#include "window.h"

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
