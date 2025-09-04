#pragma once

#include "GLFW/glfw3.h"

#include "paimon/platform/context.h"

namespace paimon {

class GlfwContext : public Context {
public:
  GlfwContext(GLFWwindow* window);
  ~GlfwContext();

  void makeCurrent() override;
  void doneCurrent() override;
private:
  GLFWwindow* m_window;
};

} // namespace paimon
