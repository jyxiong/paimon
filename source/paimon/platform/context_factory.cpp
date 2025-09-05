#include "paimon/platform/context_factory.h"

#if defined(_WIN32)
#include "paimon/platform/wgl/context.h"
#endif

namespace paimon {

std::unique_ptr<Context> createContext(const ContextFormat &format) {
#if defined(_WIN32)
  return WGLContext::create(format);
#else
  return nullptr;
#endif
}

}
