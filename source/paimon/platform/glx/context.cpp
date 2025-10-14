#ifdef PAIMON_PLATFORM_X11

#include "paimon/platform/glx/context.h"

#include <map>

#include <X11/Xlib.h>

#include "paimon/core/log/log_system.h"
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

NativeContext::NativeContext() : m_owning(true) {}

NativeContext::~NativeContext() { destroy(); }

bool NativeContext::destroy() {
  if (m_owning) {
    auto *display = GlxPlatform::instance()->display();

    if (m_context != nullptr) {
      auto currentContext = glXGetCurrentContext();
      if (currentContext == m_context) {
        doneCurrent();
      }
      glXDestroyContext(display, m_context);
      m_context = nullptr;
    }

    if (m_drawable != 0) {
      glXDestroyPbuffer(display, m_drawable);
      m_drawable = 0;
    }

    m_drawable = 0;
  }
  return true;
}

long long NativeContext::nativeHandle() const {
  return reinterpret_cast<long long>(m_context);
}

bool NativeContext::valid() const {
  return m_context != nullptr && m_drawable != 0;
}

bool NativeContext::loadGLFunctions() const {
  return gladLoaderLoadGL() != 0;
}

bool NativeContext::makeCurrent() const {
  auto *display = GlxPlatform::instance()->display();
  const auto success =
      glXMakeContextCurrent(display, m_drawable, m_drawable, m_context);
  if (!success) {
    LOG_ERROR("glXMakeContextCurrent failed");
  }
  return success;
}

bool NativeContext::doneCurrent() const {
  auto *display = GlxPlatform::instance()->display();
  const auto success = glXMakeContextCurrent(display, None, None, nullptr);
  if (!success) {
    LOG_ERROR("glXMakeContextCurrent(None, None, nullptr) failed");
  }
  return success;
}

std::unique_ptr<Context> NativeContext::getCurrent() {
  auto context = std::make_unique<NativeContext>();

  context->m_owning = false;

  context->m_context = glXGetCurrentContext();
  if (context->m_context == nullptr) {
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

std::unique_ptr<Context> NativeContext::create(const Context &shared,
                                            const ContextFormat &format) {
  auto context = std::make_unique<NativeContext>();
  context->createContext(reinterpret_cast<GLXContext>(shared.nativeHandle()),
                         format);
  return context;
}

std::unique_ptr<Context> NativeContext::create(const ContextFormat &format) {
  auto context = std::make_unique<NativeContext>();
  context->createContext(nullptr, format);
  return context;
}

void NativeContext::createContext(GLXContext shared, const ContextFormat &format) {
  Display *display = GlxPlatform::instance()->display();

  int fbCount;
  GLXFBConfig *fbConfig =
      glXChooseFBConfig(display, DefaultScreen(display), nullptr, &fbCount);
  if (fbConfig == nullptr) {
    LOG_ERROR("glXChooseFBConfig failed");
  }

  const auto contextAttributes = createContextAttributeList(format);
  m_context = glXCreateContextAttribsARB(display, fbConfig[0], shared, True,
                                         contextAttributes.data());

  const int pBufferAttributes[] = {GLX_PBUFFER_WIDTH, 1, GLX_PBUFFER_HEIGHT, 1,
                                   None};
  m_drawable = glXCreatePbuffer(display, fbConfig[0], pBufferAttributes);
}

#endif // PAIMON_PLATFORM_X11