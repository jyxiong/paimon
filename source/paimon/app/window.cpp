#include "paimon/app/window.h"

#include <glad/gl.h>

#include "paimon/core/log_system.h"
#include "paimon/app/event/application_event.h"
#include "paimon/app/event/key_event.h"
#include "paimon/app/event/mouse_event.h"

using namespace paimon;

static void windowCloseCallback(GLFWwindow* window) {
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win->getEventCallback()) {
    WindowCloseEvent event;
    win->getEventCallback()(event);
  }
}

static void windowSizeCallback(GLFWwindow* window, int width, int height) {
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win->getEventCallback()) {
    WindowResizeEvent event(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    win->getEventCallback()(event);
  }
}

static void windowFocusCallback(GLFWwindow* window, int focused) {
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win->getEventCallback()) {
    if (focused) {
      WindowFocusEvent event;
      win->getEventCallback()(event);
    } else {
      WindowLostFocusEvent event;
      win->getEventCallback()(event);
    }
  }
}

static void windowPosCallback(GLFWwindow* window, int x, int y) {
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win->getEventCallback()) {
    WindowMovedEvent event(x, y);
    win->getEventCallback()(event);
  }
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win->getEventCallback()) {
    KeyCode keyCode = static_cast<KeyCode>(key);
    if (action == GLFW_PRESS) {
      KeyPressedEvent event(keyCode, 0); // repeat count 0 for now
      win->getEventCallback()(event);
    } else if (action == GLFW_RELEASE) {
      KeyReleasedEvent event(keyCode);
      win->getEventCallback()(event);
    } else if (action == GLFW_REPEAT) {
      KeyPressedEvent event(keyCode, 1); // repeat
      win->getEventCallback()(event);
    }
  }
}

static void charCallback(GLFWwindow* window, unsigned int codepoint) {
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win->getEventCallback()) {
    KeyTypedEvent event(static_cast<KeyCode>(codepoint));
    win->getEventCallback()(event);
  }
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win->getEventCallback()) {
    MouseCode mouseCode = static_cast<MouseCode>(button);
    if (action == GLFW_PRESS) {
      MouseButtonPressedEvent event(mouseCode);
      win->getEventCallback()(event);
    } else if (action == GLFW_RELEASE) {
      MouseButtonReleasedEvent event(mouseCode);
      win->getEventCallback()(event);
    }
  }
}

static void cursorPosCallback(GLFWwindow* window, double x, double y) {
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win->getEventCallback()) {
    MouseMovedEvent event(static_cast<float>(x), static_cast<float>(y));
    win->getEventCallback()(event);
  }
}

static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
  Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (win->getEventCallback()) {
    MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
    win->getEventCallback()(event);
  }
}

using namespace paimon;

int toGlfw(bool value) { return value ? GLFW_TRUE : GLFW_FALSE; }

int toGlfw(ContextProfile profile) {
  switch (profile) {
  case ContextProfile::Core:
    return GLFW_OPENGL_CORE_PROFILE;
  case ContextProfile::Compatibility:
    return GLFW_OPENGL_COMPAT_PROFILE;
  default:
    return GLFW_OPENGL_ANY_PROFILE;
  }
}

Window::Window() {
  if (!glfwInit()) {
    LOG_ERROR("glfwInit failed");
    return;
  }

  glfwSetErrorCallback([](int error, const char *description) {
    LOG_ERROR("GLFW error {}: {}", error, description);
  });
}

Window::~Window() {
  destroy();

  glfwTerminate();
}

void Window::resize(uint32_t width, uint32_t height) {
  glfwSetWindowSize(m_window, width, height);
}

void Window::pollEvents() { glfwPollEvents(); }

void Window::swapBuffers() { glfwSwapBuffers(m_window); }

bool Window::shouldClose() const { return glfwWindowShouldClose(m_window); }

void Window::destroy() {
  glfwDestroyWindow(m_window);
  m_window = nullptr;
}

std::unique_ptr<Window> Window::create(const WindowConfig &config) {
  auto window = std::make_unique<Window>();

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_OPENGL_PROFILE, toGlfw(config.format.profile));
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.format.majorVersion);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.format.minorVersion);
  glfwWindowHint(GLFW_CONTEXT_DEBUG, toGlfw(config.format.debug));
  glfwWindowHint(GLFW_RESIZABLE, toGlfw(config.resizable));
  glfwWindowHint(GLFW_VISIBLE, toGlfw(config.visible));

  window->m_window = glfwCreateWindow(
      config.width, config.height, config.title.c_str(),
      config.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

  if (!window->m_window) {
    LOG_ERROR("Failed to create GLFW window");
    return nullptr;
  }

  glfwSetWindowUserPointer(window->m_window, window.get());

  glfwSetWindowCloseCallback(window->m_window, windowCloseCallback);
  glfwSetWindowSizeCallback(window->m_window, windowSizeCallback);
  glfwSetWindowFocusCallback(window->m_window, windowFocusCallback);
  glfwSetWindowPosCallback(window->m_window, windowPosCallback);
  glfwSetKeyCallback(window->m_window, keyCallback);
  glfwSetCharCallback(window->m_window, charCallback);
  glfwSetMouseButtonCallback(window->m_window, mouseButtonCallback);
  glfwSetCursorPosCallback(window->m_window, cursorPosCallback);
  glfwSetScrollCallback(window->m_window, scrollCallback);

  glfwMakeContextCurrent(window->m_window);

  gladLoadGL(glfwGetProcAddress);

  return window;
}

bool Window::isKeyPressed(KeyCode key) const {
  int state = glfwGetKey(m_window, to_int(key));
  return state == GLFW_PRESS;
}

bool Window::isMouseButtonPressed(MouseCode button) const {
  int state = glfwGetMouseButton(m_window, to_int(button));
  return state == GLFW_PRESS;
}

std::pair<double, double> Window::getMousePosition() const {
  double x, y;
  glfwGetCursorPos(m_window, &x, &y);
  return {x, y};
}
