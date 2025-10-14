#include "paimon/core/log/log_system.h"
#include "paimon/opengl/query.h"

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

using namespace paimon;

namespace {
glm::ivec2 g_size = {800, 600};
} // namespace

void error(int errnum, const char *errmsg) {
  LOG_ERROR("GLFW error {}: {}", errnum, errmsg);
}

void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height) {
  g_size = glm::ivec2{width, height};
}

void key_callback(GLFWwindow *window, int key, int /*scancode*/, int action,
                  int /*modes*/) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    glfwSetWindowShouldClose(window, true);
}

void glfwError(int error_code, const char *description) {
  LOG_INFO("GLFW error {}: {}", error_code, description);
}

int main() {
  LogSystem::init();

  if (!glfwInit()) {
    LOG_ERROR("Failed to initialize GLFW");
    return -1;
  }

  glfwSetErrorCallback(error);

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  // Create a context and, if valid, make it current
  GLFWwindow *window =
      glfwCreateWindow(640, 480, "paimon Texture", nullptr, nullptr);
  if (window == nullptr) {
    LOG_ERROR("GLFW window creation failed. Terminate execution.");

    glfwTerminate();
    return -1;
  }
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwMakeContextCurrent(window);

  if (!gladLoadGL(glfwGetProcAddress)) {
    LOG_ERROR("Failed to initialize GLAD");
    return -1;
  }

  Query query(GL_TIME_ELAPSED);
  if (!query.is_valid()) {
    LOG_ERROR("Failed to create query object");
    return -1;
  }

    // 演示一次 OpenGL 查询过程
    query.begin();
    // 执行一段 OpenGL 操作，比如清屏
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    query.end();

    // 获取查询结果（阻塞直到结果可用）
    GLuint64 elapsed = query.get<GLuint64>(GL_QUERY_RESULT);
    LOG_INFO("GL_TIME_ELAPSED: {} ns", elapsed);



  glfwTerminate();
  return 0;
}