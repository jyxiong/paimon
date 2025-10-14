#include "paimon/platform/glfw/context.h"

#include <map>

#include "GLFW/glfw3.h"
#include "glad/gl.h"
#include "paimon/core/log/log_system.h"


using namespace paimon;

std::map<int, int> createContextAttributeList(const ContextFormat &format) {
  std::map<int, int> attributes;

  if (format.versionMajor > 0) {
    attributes[GLFW_CONTEXT_VERSION_MAJOR] = format.versionMajor;
    attributes[GLFW_CONTEXT_VERSION_MINOR] = format.versionMinor;
  }

  if (format.debug) {
    attributes[GLFW_CONTEXT_DEBUG] = GLFW_TRUE;
  }

  switch (format.profile) {
  case ContextProfile::Core:
    attributes[GLFW_OPENGL_PROFILE] = GLFW_OPENGL_CORE_PROFILE;
    break;
  case ContextProfile::Compatibility:
    attributes[GLFW_OPENGL_PROFILE] = GLFW_OPENGL_COMPAT_PROFILE;
    break;
  default:
    break;
  }

  return std::move(attributes);
}

GlfwContext::GlfwContext() : m_owning(true) {}

GlfwContext::~GlfwContext() { destroy(); }

bool GlfwContext::destroy() {
  if (m_owning && m_context != nullptr) {
    auto currentContext = glfwGetCurrentContext();
    if (currentContext == m_context) {
      doneCurrent();
    }
    glfwDestroyWindow(m_context);
    m_context = nullptr;
  }
  return true;
}

long long GlfwContext::nativeHandle() const {
  return reinterpret_cast<long long>(m_context);
}

bool GlfwContext::valid() const { return m_context != nullptr; }

bool GlfwContext::loadGLFunctions() const {
  return gladLoadGL(glfwGetProcAddress) != 0;
}

bool GlfwContext::makeCurrent() const {
  glfwMakeContextCurrent(m_context);
  return true;
}

bool GlfwContext::doneCurrent() const {
  glfwMakeContextCurrent(nullptr);
  return true;
}

std::unique_ptr<Context> GlfwContext::getCurrent() {
  auto context = std::make_unique<GlfwContext>();

  context->m_owning = false;

  context->m_context = glfwGetCurrentContext();
  if (context->m_context == nullptr) {
    LOG_ERROR("glfwGetCurrentContext failed");
    return nullptr;
  }

  return context;
}

std::unique_ptr<Context> GlfwContext::create(const Context &shared,
                                             const ContextFormat &format) {
  if (!shared.valid()) {
    LOG_ERROR("Shared context is not a GlfwContext");
    return nullptr;
  }

  auto context = std::make_unique<GlfwContext>();

  context->createContext(reinterpret_cast<GLFWwindow *>(shared.nativeHandle()),
                         format);

  return context;
}

std::unique_ptr<Context> GlfwContext::create(const ContextFormat &format) {
  auto context = std::make_unique<GlfwContext>();

  context->createContext(nullptr, format);

  return context;
}

void GlfwContext::createContext(GLFWwindow *shared,
                                const ContextFormat &format) {
  glfwDefaultWindowHints();

  auto attributes = createContextAttributeList(format);
  for (const auto &[hint, value] : attributes) {
    glfwWindowHint(hint, value);
  }

  m_context = glfwCreateWindow(640, 480, "Paimon", nullptr, shared);
  if (m_context == nullptr) {
    LOG_ERROR("glfwCreateWindow failed");
    return;
  }
}
