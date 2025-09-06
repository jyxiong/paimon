#include "paimon/core/log/log_system.h"
#include "paimon/platform/context_factory.h"

#include <glad/gl.h>
#include <iostream>

using namespace paimon;

int main() {
  LogSystem::init();

  auto context = createContext(ContextFormat{.versionMajor = 4,
                                             .versionMinor = 6,
                                             .profile = ContextProfile::Core,
                                             .debug = true});

  context->makeCurrent();
  const auto versionString =
      reinterpret_cast<const char *>(glGetString(GL_VERSION));
  std::cout << "Created context with version " << versionString << std::endl;

  context->doneCurrent();

  return 0;
}