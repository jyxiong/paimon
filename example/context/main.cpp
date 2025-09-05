#include "paimon/core/log/log_system.h"
#include "paimon/platform/context_factory.h"

using namespace paimon;

int main() {
  LogSystem::init();

  auto context = createContext(ContextFormat{.versionMajor = 4,
                                      .versionMinor = 6,
                                      .profile = ContextProfile::Core,
                                      .debug = true});

  if (context != nullptr) {
    context->makeCurrent();
    context->doneCurrent();
  }

  return 0;
}