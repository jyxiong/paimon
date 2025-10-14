#pragma once

#include "paimon/platform/context.h"

#include "GLFW/glfw3.h"

namespace paimon {

class GlfwContext : public Context {
public:
  GlfwContext();
  ~GlfwContext();

  bool destroy() override;

  long long nativeHandle() const override;

  bool valid() const override;

  bool loadGLFunctions() const override;

  bool makeCurrent() const override;

  bool doneCurrent() const override;

  static std::unique_ptr<Context> getCurrent();

  static std::unique_ptr<Context> create(const Context& shared, const ContextFormat &format);

  static std::unique_ptr<Context> create(const ContextFormat &format);

private:
  void createContext(GLFWwindow* shared, const ContextFormat &format);

private:
  GLFWwindow* m_context = nullptr;
  bool m_owning;
};

} // namespace paimon