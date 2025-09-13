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

void loadGLFunctions(const Context &context) {
  context.makeCurrent();

  static std::once_flag glad_flag;
  std::call_once(glad_flag, []() {
    if (!gladLoaderLoadGL()) {
      LOG_ERROR("Failed to load OpenGL functions");
    }

    auto glVersion = glGetString(GL_VERSION);
    auto glRenderer = glGetString(GL_RENDERER);
    auto glVendor = glGetString(GL_VENDOR);
    LOG_INFO("Loaded OpenGL functions");
    LOG_INFO("  OpenGL Version: {}", reinterpret_cast<const char *>(glVersion));
    LOG_INFO("  OpenGL Renderer: {}",
             reinterpret_cast<const char *>(glRenderer));
    LOG_INFO("  OpenGL Vendor: {}", reinterpret_cast<const char *>(glVendor));
  });

  context.doneCurrent();
}

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

  loadGLFunctions(*context);

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

  loadGLFunctions(*context);

  return context;
}

std::unique_ptr<Context> ContextFactory::createContext(const Context& shared, const ContextFormat &format) {
  std::unique_ptr<Context> context;
#if defined(_WIN32)
  context = WGLContext::create(shared, format);
#else
  // context = GlxContext::create(shared, format);
  context = EglContext::create(shared, format);
#endif

  loadGLFunctions(*context);

  return context;

}

} // namespace paimon
