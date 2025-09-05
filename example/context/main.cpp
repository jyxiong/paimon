#include "paimon/core/base/macro.h"
#include "paimon/platform/wgl/context.h"


using namespace paimon;

int main() {
  LogSystem::init();

  auto context =
      WGLContext::create(ContextFormat{.versionMajor = 4,
                                       .versionMinor = 6,
                                       .profile = ContextProfile::Core,
                                       .debug = true});

  context->makeCurrent();
  context->doneCurrent();

  return 0;
}