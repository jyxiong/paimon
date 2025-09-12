#include "glad/gl.h"

#include "paimon/core/base/macro.h"
#include "paimon/platform/context_factory.h"

using namespace paimon;

void workerThread1(Context *shared,
                   const ContextFormat &format = ContextFormat()) {
  auto context = ContextFactory::createContext(*shared, format);
  if (!context->valid()) {
    LOG_ERROR("Worker 1: failed to create shared context");
    return;
  }

  context->makeCurrent();

  const auto versionString =
      reinterpret_cast<const char *>(glGetString(GL_VERSION));
  LOG_INFO("Worker 1: created shared context with version {}", versionString);

  context->doneCurrent();
}

void workerThread2(Context *context) {
  context->makeCurrent();

  const auto versionString =
      reinterpret_cast<const char *>(glGetString(GL_VERSION));
  LOG_INFO("Worker 2: using shared context with version {}", versionString);

  context->doneCurrent();
}

int main() {
  auto context = ContextFactory::createContext(
      ContextFormat{.versionMajor = 3,
                    .versionMinor = 3,
                    .profile = ContextProfile::Core,
                    .debug = true});
  if (!context->valid()) {
    LOG_ERROR("Failed to create main context");
    return EXIT_FAILURE;
  }

  context->makeCurrent();

  const auto versionString =
      reinterpret_cast<const char *>(glGetString(GL_VERSION));
  LOG_INFO("Main: created main context with version {}", versionString);

  context->doneCurrent();

  //
  // Worker 1 receives a pointer to the main context and creates its own shared
  // context
  //
  auto worker1 = std::thread(&workerThread1, context.get());

  //
  // Worker 2 receives a pointer to a shared context created on the main thread
  //
  auto worker2Context = ContextFactory::createContext(
      *context, ContextFormat{.versionMajor = 3,
                             .versionMinor = 3,
                             .profile = ContextProfile::Core,
                             .debug = true});

  if (!worker2Context->valid()) {
    LOG_ERROR("Worker 2: failed to create shared context");
    return EXIT_FAILURE;
  }

  auto worker2 = std::thread(&workerThread2, worker2Context.get());

  worker1.join();
  worker2.join();

  return EXIT_SUCCESS;
}
