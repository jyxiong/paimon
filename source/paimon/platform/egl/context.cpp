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

EglContext::EglContext()
    : m_surface(EGL_NO_SURFACE),
      m_context(EGL_NO_CONTEXT), m_owning(true) {}

EglContext::~EglContext() { destroy(); }

bool EglContext::destroy() {
  if (m_owning) {
    if (m_context != EGL_NO_CONTEXT) {
      if (!eglDestroyContext(EglPlatform::instance()->display(),
                                       m_context)) {
        LOG_ERROR("Failed to destroy EGL context");
        return false;
      }

      m_context = EGL_NO_CONTEXT;
    }
  }
  return true;
}

long long EglContext::nativeHandle() const {
  return reinterpret_cast<long long>(m_context);
}

bool EglContext::valid() const { return m_context != EGL_NO_CONTEXT; }

bool EglContext::makeCurrent() const {
  if (!eglMakeCurrent(EglPlatform::instance()->display(), m_surface,
                     m_surface, m_context)) {
    LOG_ERROR("Failed to make EGL context current");
    return false;
  }
  return true;
}

bool EglContext::doneCurrent() const {
  if (!eglMakeCurrent(EglPlatform::instance()->display(),
                                EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
    LOG_ERROR("Failed to release EGL context");
    return false;
  }
  return true;
}

std::unique_ptr<Context> EglContext::getCurrent() {
  auto context = std::make_unique<EglContext>();

  context->m_owning = false;
  context->m_context = eglGetCurrentContext();
  if (context->m_context == EGL_NO_CONTEXT) {
    return nullptr;
  }

  context->m_surface = eglGetCurrentSurface(EGL_DRAW);
  if (context->m_surface == EGL_NO_SURFACE) {
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
  if (!eglChooseConfig(display, configAttributes, &config, 1, &numConfigs)) {
    LOG_ERROR("Failed to choose EGL config");
    return;
  }

  const auto contextAttributes = createContextAttributeList(format);
  m_context =
      eglCreateContext(display, config, shared, contextAttributes.data());
  if (m_context == EGL_NO_CONTEXT) {
    LOG_ERROR("Failed to create EGL context");
    return;
  }
}

#endif // __linux__