#include "paimon/app/window.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/debug_message.h"

using namespace paimon;

int main() {
  LogSystem::init();

  auto window = Window::create(WindowConfig{
      .title = "Debug Message Example",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 5,
              .profile = ContextProfile::Core,
          },
      .width = 800,
      .height = 600,

  });

  DebugMessage::enable();
  DebugMessage::setSynchronous(true);

  DebugMessage::insert({GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
                        GL_DEBUG_SEVERITY_HIGH, "Fist debug message"});

  DebugMessage::insert({GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 2,
                        GL_DEBUG_SEVERITY_MEDIUM, "Second debug message"});

  DebugMessage::insert({GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 3,
                        GL_DEBUG_SEVERITY_LOW, "Third debug message"});

  while (!window->shouldClose()) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    window->swapBuffers();
    window->pollEvents();
  }

  window->destroy();

  return 0;
}