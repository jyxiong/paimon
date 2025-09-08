#include "paimon/platform/context_factory.h"

#include "glad/gl.h"

#include "paimon/core/base/macro.h"
#include <memory>
#if defined(_WIN32)
#include "paimon/platform/wgl/context.h"
#else
#include "paimon/platform/glx/context.h"
#include "paimon/platform/egl/context.h"
#endif

namespace paimon {

std::unique_ptr<Context> ContextFactory::getCurrentContext() {
  std::unique_ptr<Context> context;
#if defined(_WIN32)
  context = WGLContext::getCurrent();
#else
  // context = GlxContext::getCurrent();
  context = EglContext::getCurrent();
#endif
  if (context == nullptr) {
    LOG_ERROR("Failed to get current context");
    return nullptr;
  }

  auto success = gladLoaderLoadGL();
  if (!success) {
    LOG_ERROR("Failed to load OpenGL functions");
  }

  return context;
}

std::unique_ptr<Context> ContextFactory::createContext(const ContextFormat &format) {

  std::unique_ptr<Context> context;
#if defined(_WIN32)
  context = WGLContext::create(format);
#else
  // context = GlxContext::create(format);
  context = EglContext::create(format);
#endif

  context->makeCurrent();

  auto success = gladLoaderLoadGL();
  if (!success) {
    LOG_ERROR("Failed to load OpenGL functions");
  }

  return context;
}

} // namespace paimon
