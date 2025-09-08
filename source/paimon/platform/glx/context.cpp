#ifdef __linux__

#include "paimon/platform/glx/context.h"

#include <map>

#include <X11/Xlib.h>

#include "paimon/core/base/macro.h"
#include "paimon/platform/glx/platform.h"

using namespace paimon;

namespace {

std::vector<int> createContextAttributeList(const ContextFormat &format) {
  std::map<int, int> attributes;

  if (format.versionMajor > 0) {
    attributes[GLX_CONTEXT_MAJOR_VERSION_ARB] = format.versionMajor;
    attributes[GLX_CONTEXT_MINOR_VERSION_ARB] = format.versionMinor;
  }

  if (format.debug) {
    attributes[GLX_CONTEXT_FLAGS_ARB] = GLX_CONTEXT_DEBUG_BIT_ARB;
  }

  switch (format.profile) {
  case ContextProfile::Core:
    attributes[GLX_CONTEXT_PROFILE_MASK_ARB] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
    break;
  case ContextProfile::Compatibility:
    attributes[GLX_CONTEXT_PROFILE_MASK_ARB] =
        GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
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
  list.push_back(None); // finalize list

  return std::move(list);
}
} // namespace

GlxContext::GlxContext() : m_owning(true) {}

GlxContext::~GlxContext() {}

bool GlxContext::destroy() {
  if (m_owning) {
    auto *display = GlxPlatform::instance()->display();

    if (m_contextHandle != nullptr) {
      auto currentContext = glXGetCurrentContext();
      if (currentContext == m_contextHandle) {
        doneCurrent();
      }
      glXDestroyContext(display, m_contextHandle);
      m_contextHandle = nullptr;
    }

    if (m_pBuffer != 0) {
      glXDestroyPbuffer(display, m_pBuffer);
      m_pBuffer = 0;
    }

    m_drawable = 0;
  }
  return true;
}

long long GlxContext::nativeHandle() {
  return reinterpret_cast<long long>(m_contextHandle);
}

bool GlxContext::valid() {
  return m_contextHandle != nullptr && m_drawable != 0;
}

bool GlxContext::makeCurrent() {
  auto *display = GlxPlatform::instance()->display();
  const auto success =
      glXMakeContextCurrent(display, m_drawable, m_drawable, m_contextHandle);
  if (!success) {
    LOG_ERROR("glXMakeContextCurrent failed");
  }
  return success;
}

bool GlxContext::doneCurrent() {
  auto *display = GlxPlatform::instance()->display();
  const auto success = glXMakeContextCurrent(display, None, None, nullptr);
  if (!success) {
    LOG_ERROR("glXMakeContextCurrent(None, None, nullptr) failed");
  }
  return success;
}

std::unique_ptr<Context> GlxContext::getCurrent() {
  auto context = std::make_unique<GlxContext>();

  context->m_owning = false;

  context->m_contextHandle = glXGetCurrentContext();
  if (context->m_contextHandle == nullptr) {
    LOG_ERROR("glXGetCurrentContext failed");
    return nullptr;
  }

  context->m_drawable = glXGetCurrentDrawable();
  if (context->m_drawable == 0) {
    LOG_ERROR("glXGetCurrentDrawable failed");
    return nullptr;
  }

  return context;
}

std::unique_ptr<Context> GlxContext::create(const ContextFormat &format) {
  auto context = std::make_unique<GlxContext>();
  context->createContext(nullptr, format);
  return context;
}

void GlxContext::createContext(GLXContext shared, const ContextFormat &format) {
  Display *display = GlxPlatform::instance()->display();

  int fbCount;
  GLXFBConfig *fbConfig =
      glXChooseFBConfig(display, DefaultScreen(display), nullptr, &fbCount);
  if (fbConfig == nullptr) {
    LOG_ERROR("glXChooseFBConfig failed");
  }

  const auto contextAttributes = createContextAttributeList(format);
  m_contextHandle = glXCreateContextAttribsARB(display, fbConfig[0], shared,
                                               True, contextAttributes.data());
  XSync(display, false);
  // if (m_contextHandle == nullptr || xErrorHandler.errorCode() != Success) {
  //     throw InternalException(Error::INVALID_CONFIGURATION,
  //     "glXCreateContextAttribsARB returned nullptr (" +
  //     xErrorHandler.errorString() + ")");
  // }

  const int pBufferAttributes[] = {GLX_PBUFFER_WIDTH, 1, GLX_PBUFFER_HEIGHT, 1,
                                   None};
  m_pBuffer = glXCreatePbuffer(display, fbConfig[0], pBufferAttributes);

  XSync(display, false);

  // check if pbuffer is supported
  const auto success =
      glXMakeContextCurrent(display, m_pBuffer, m_pBuffer, m_contextHandle);
  if (success) {
    glXMakeContextCurrent(display, None, None, nullptr);
    m_drawable = m_pBuffer;
  } else {
    m_drawable = DefaultRootWindow(display);
  }
}

#endif // __linux__