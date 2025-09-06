#include "paimon/core/log/log_system.h"
#include "paimon/platform/context_factory.h"

using namespace paimon;

int main() {
  LogSystem::init();

  auto context = createContext(ContextFormat{.versionMajor = 3,
                                             .versionMinor = 3,
                                             .profile = ContextProfile::Core,
                                             .debug = true});

  context->makeCurrent();
  context->doneCurrent();

  return 0;
}