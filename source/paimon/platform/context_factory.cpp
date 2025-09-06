#include "paimon/platform/context_factory.h"

#if defined(_WIN32)
#include "paimon/platform/wgl/context.h"
#else
#include "paimon/platform/glx/context.h"
#endif

namespace paimon {

std::unique_ptr<Context> createContext(const ContextFormat &format) {
#if defined(_WIN32)
  return WGLContext::create(format);
#else
  return GlxContext::create(format);
#endif
}

}
