#ifdef PAIMON_PLATFORM_EGL

#include "paimon/platform/egl/context.h"

#include <map>

#include "paimon/core/log/log_system.h"
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

std::string eglGetErrorString(EGLint error) {
  switch (error) {
  case EGL_SUCCESS:
    return "EGL_SUCCESS";
  case EGL_NOT_INITIALIZED:
    return "EGL_NOT_INITIALIZED";
  case EGL_BAD_ACCESS:
    return "EGL_BAD_ACCESS";
  case EGL_BAD_ALLOC:
    return "EGL_BAD_ALLOC";
  case EGL_BAD_ATTRIBUTE:
    return "EGL_BAD_ATTRIBUTE";
  case EGL_BAD_CONFIG:
    return "EGL_BAD_CONFIG";
  case EGL_BAD_CONTEXT:
    return "EGL_BAD_CONTEXT";
  case EGL_BAD_CURRENT_SURFACE:
    return "EGL_BAD_CURRENT_SURFACE";
  case EGL_BAD_DISPLAY:
    return "EGL_BAD_DISPLAY";
  case EGL_BAD_MATCH:
    return "EGL_BAD_MATCH";
  case EGL_BAD_NATIVE_PIXMAP:
    return "EGL_BAD_NATIVE_PIXMAP";
  case EGL_BAD_NATIVE_WINDOW:
    return "EGL_BAD_NATIVE_WINDOW";
  case EGL_BAD_PARAMETER:
    return "EGL_BAD_PARAMETER";
  case EGL_BAD_SURFACE:
    return "EGL_BAD_SURFACE";
  case EGL_CONTEXT_LOST:
    return "EGL_CONTEXT_LOST";
  default:
    return "Unknown EGL error";
  }
}

EGLint checkEglError(const std::string &msg) {
  auto error = eglGetError();
  if (error != EGL_SUCCESS) {
    LOG_ERROR("{}: {}", msg, eglGetErrorString(error));
  }
  return error;
}

} // namespace

NativeContext::NativeContext()
    : m_surface(EGL_NO_SURFACE), m_context(EGL_NO_CONTEXT), m_owning(true) {}

NativeContext::~NativeContext() { destroy(); }

bool NativeContext::destroy() {
  if (m_owning) {
    if (m_context != EGL_NO_CONTEXT) {
      if (!eglDestroyContext(EglPlatform::instance()->display(), m_context)) {
        LOG_ERROR("Failed to destroy EGL context");
        return false;
      }

      m_context = EGL_NO_CONTEXT;
    }
  }
  return true;
}

long long NativeContext::nativeHandle() const {
  return reinterpret_cast<long long>(m_context);
}

bool NativeContext::valid() const { return m_context != EGL_NO_CONTEXT; }

bool NativeContext::loadGLFunctions() const {
  return gladLoaderLoadGL() != 0;
}

bool NativeContext::makeCurrent() const {
  auto success = eglMakeCurrent(EglPlatform::instance()->display(), m_surface,
                                m_surface, m_context);
  if (!success) {
    LOG_ERROR("Failed to make EGL context current");
  }
  return success;
}

bool NativeContext::doneCurrent() const {
  auto success = eglMakeCurrent(EglPlatform::instance()->display(),
                                EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

  if (!success) {
    LOG_ERROR("Failed to release EGL context");
  }
  return success;
}

std::unique_ptr<Context> NativeContext::getCurrent() {
  auto context = std::make_unique<NativeContext>();

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

std::unique_ptr<Context> NativeContext::create(const Context &shared,
                                            const ContextFormat &format) {
  auto context = std::make_unique<NativeContext>();

  context->createContext(reinterpret_cast<EGLContext>(shared.nativeHandle()),
                         format);
  return context;
}

std::unique_ptr<Context> NativeContext::create(const ContextFormat &format) {
  auto context = std::make_unique<NativeContext>();
  context->createContext(EGL_NO_CONTEXT, format);
  return context;
}

void NativeContext::createContext(EGLContext shared, const ContextFormat &format) {
  auto display = EglPlatform::instance()->display();

  // IMPORTANT: eglBindAPI is thread-local and must be called in each thread before creating an OpenGL context.
  if (eglQueryAPI() != EGL_OPENGL_API) {
    eglBindAPI(EGL_OPENGL_API);
  }

  static EGLint configAttributes[] = {EGL_SURFACE_TYPE, 0,
                                      EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
                                      EGL_NONE};
  EGLint numConfigs;
  EGLConfig config;
  if (!eglChooseConfig(display, configAttributes, &config, 1, &numConfigs)) {
    checkEglError("eglChooseConfig");
    LOG_ERROR("Failed to choose EGL config");
    return;
  }

  const auto contextAttributes = createContextAttributeList(format);
  m_context =
      eglCreateContext(display, config, shared, contextAttributes.data());
  if (m_context == EGL_NO_CONTEXT) {
    checkEglError("eglCreateContext");
    LOG_ERROR("Failed to create EGL context");
    return;
  }
}

#endif // PAIMON_PLATFORM_EGL