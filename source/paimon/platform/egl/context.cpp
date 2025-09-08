#ifdef __linux__

#include "paimon/platform/egl/context.h"

#include <map>

#include "paimon/core/base/macro.h"
#include "paimon/platform/egl/platform.h"

using namespace paimon;

namespace {
std::vector<int> createContextAttributeList(const ContextFormat &format) {
  std::map<int, int> attributes;

  if (format.versionMajor > 0) {
    attributes[EGL_CONTEXT_MAJOR_VERSION] = format.versionMajor;
    attributes[EGL_CONTEXT_MINOR_VERSION] = format.versionMinor;
  }

  if (format.debug) {
      attributes[EGL_CONTEXT_OPENGL_DEBUG] = EGL_TRUE;
  }

  switch (format.profile) {
  case ContextProfile::Core:
    attributes[EGL_CONTEXT_OPENGL_PROFILE_MASK] =
        EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;
    break;
  case ContextProfile::Compatibility:
    attributes[EGL_CONTEXT_OPENGL_PROFILE_MASK] =
        EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT;
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
  list.push_back(EGL_NONE); // finalize list

  return std::move(list);
}

} // namespace

EglContext::EglContext() : m_contextHandle(EGL_NO_CONTEXT), m_owning(true) {}

EglContext::~EglContext() {}

bool EglContext::destroy() {
  if (m_owning && m_contextHandle != EGL_NO_CONTEXT) {
    auto success =
        eglDestroyContext(EglPlatform::instance()->display(), m_contextHandle);
    if (!success) {
      LOG_ERROR("Failed to destroy EGL context");
      return false;
    }

    m_contextHandle = EGL_NO_CONTEXT;
  }
  return true;
}

long long EglContext::nativeHandle() {
  return reinterpret_cast<long long>(m_contextHandle);
}

bool EglContext::valid() { return m_contextHandle != EGL_NO_CONTEXT; }

bool EglContext::makeCurrent() {

  auto success = eglMakeCurrent(EglPlatform::instance()->display(), EGL_NO_SURFACE,
                                EGL_NO_SURFACE, m_contextHandle);

  if (!success) {
    LOG_ERROR("Failed to make EGL context current");
    return false;
  }
  return true;
}

bool EglContext::doneCurrent() {
  auto success = eglMakeCurrent(EglPlatform::instance()->display(), EGL_NO_SURFACE,
                                EGL_NO_SURFACE, EGL_NO_CONTEXT);
  if (!success) {
    LOG_ERROR("Failed to release EGL context");
    return false;
  }
  return true;
}

std::unique_ptr<Context> EglContext::getCurrent() {
  auto context = std::make_unique<EglContext>();

  context->m_owning = false;
  context->m_contextHandle = eglGetCurrentContext();
  if (context->m_contextHandle == EGL_NO_CONTEXT) {
    return nullptr;
  }

  return context;
}

std::unique_ptr<Context> EglContext::create(const ContextFormat &format) {
  auto context = std::make_unique<EglContext>();
  context->createContext(EGL_NO_CONTEXT, format);
  return context;
}

void EglContext::createContext(EGLContext shared, const ContextFormat &format) {
  auto display = EglPlatform::instance()->display();

  static EGLint configAttributes[] = {EGL_SURFACE_TYPE, 0, EGL_RENDERABLE_TYPE,
                                      EGL_OPENGL_BIT, EGL_NONE};
  EGLint numConfigs;
  EGLConfig config;
  auto success =
      eglChooseConfig(display, configAttributes, &config, 1, &numConfigs);
  if (!success) {
    LOG_ERROR("Failed to choose EGL config");
    return;
  }

  const auto contextAttributes = createContextAttributeList(format);
  m_contextHandle =
      eglCreateContext(display, config, shared, contextAttributes.data());
  if (m_contextHandle == EGL_NO_CONTEXT) {
    LOG_ERROR("Failed to create EGL context");
    return;
  }
}

#endif // __linux__