#include "paimon/platform/context_factory.h"

#include "glad/gl.h"

#include "paimon/core/base/macro.h"
#include <memory>
#if defined(_WIN32)
#include "paimon/platform/wgl/context.h"
#else
#include "paimon/platform/glx/context.h"
#endif

namespace paimon {

std::unique_ptr<Context> createContext(const ContextFormat &format) {

  std::unique_ptr<Context> context;
#if defined(_WIN32)
  context = WGLContext::create(format);
#else
  context = GlxContext::create(format);
#endif

  context->makeCurrent();

  auto success = gladLoaderLoadGL();
  if (!success) {
    LOG_ERROR("Failed to load OpenGL functions");
  }

  return context;
}

} // namespace paimon
