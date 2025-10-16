#include "paimon/core/log_system.h"

#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "paimon/opengl/debug_message.h"

using namespace paimon;

int main() {
  LogSystem::init();

  if (!glfwInit()) {
    LOG_ERROR("GLFW initialization failed. Terminate execution.");

    return 1;
  }


  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  // Create a context and, if valid, make it current
  GLFWwindow *window =
      glfwCreateWindow(640, 480, "globjects Texture", nullptr, nullptr);
  if (window == nullptr) {
    LOG_ERROR("GLFW window creation failed. Terminate execution.");

    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    LOG_ERROR("Failed to initialize OpenGL context");

    return -1;
  }

  DebugMessage::enable();
  DebugMessage::setSynchronous(true);

  DebugMessage::insert({GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
                        GL_DEBUG_SEVERITY_HIGH, "Fist debug message"});

  DebugMessage::insert({GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 2,
                        GL_DEBUG_SEVERITY_MEDIUM, "Second debug message"});

  DebugMessage::insert({GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 3,
                        GL_DEBUG_SEVERITY_LOW, "Third debug message"});

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}