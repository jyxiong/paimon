#include "paimon/platform/context_factory.h"

#include "paimon/platform/wgl/context.h"

using namespace paimon;

std::unique_ptr<Context> create(const ContextFormat &format) {
  return WGLContext::create(format);
}