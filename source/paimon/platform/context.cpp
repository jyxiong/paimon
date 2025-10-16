#include "paimon/platform/context.h"

#include <cassert>

#include "paimon/core/log_system.h"

#if defined(PAIMON_PLATFORM_WIN32)
#include "paimon/platform/wgl_context/context.h"
#elif defined(PAIMON_PLATFORM_X11)
#include "paimon/platform/glx_context/context.h"
#elif defined(PAIMON_PLATFORM_EGL)
#include "paimon/platform/egl_context/context.h"
#endif

using namespace paimon;

std::once_flag Context::s_gladLoadFlag;

Context::Context() : m_threadId(std::this_thread::get_id()) {}

Context::~Context() {
  if (std::this_thread::get_id() != m_threadId) {
    LOG_ERROR("Context destroyed from wrong thread");
  }
}

std::unique_ptr<Context> Context::getCurrent() {
  auto context = NativeContext::getCurrent();
  if (context == nullptr) {
    LOG_ERROR("Failed to get current context");
    return nullptr;
  }

  context->init();
  return context;
}

std::unique_ptr<Context> Context::create(const ContextFormat &format) {
  auto context = NativeContext::create(format);
  context->init();
  return context;
}

std::unique_ptr<Context> Context::create(const Context &shared,
                                         const ContextFormat &format) {
  auto context = NativeContext::create(shared, format);
  context->init();
  return context;
}

void Context::init() {
  // IMPORTANT:
  // On many platforms/drivers, a newly created OpenGL context (including shared
  // contexts) MUST be made current at least once in the thread where it was
  // created before being used or made current in other threads. Failing to do
  // so may result in undefined behavior or crashes. Always makeCurrent() in the
  // creation thread before passing the context to others.
  makeCurrent();

  std::call_once(s_gladLoadFlag, [&]() {
    if (!loadGLFunctions()) {
      LOG_ERROR("Failed to load OpenGL functions");
    }
  });

  doneCurrent();
}
