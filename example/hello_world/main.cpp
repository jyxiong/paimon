#include "paimon/core/base/macro.h"

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "paimon/opengl/program.h"

using namespace paimon;

namespace {
// hard code triangle vertices in shader for simplicity
std::string vertex_spurce = R"(
  #version 460 core
  void main()
  {
    vec3 positions[3] = vec3[](
      vec3(0.0, 0.5, 0.0),
      vec3(-0.5, -0.5, 0.0),
      vec3(0.5, -0.5, 0.0)
    );
    gl_Position = vec4(positions[gl_VertexID], 1.0);
  }
  )";

std::string fragment_source = R"(
  #version 460 core
  out vec4 FragColor;
  void main() 
  {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  }
  )";

glm::ivec2 g_size = {};
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
    LOG_ERROR("GLFW initialization failed. Terminate execution.");

    return 1;
  }

  glfwSetErrorCallback(error);

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
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    LOG_ERROR("Failed to initialize OpenGL context");

    return -1;
  }

  Shader vertex_shader(GL_VERTEX_SHADER);
  Shader fragment_shader(GL_FRAGMENT_SHADER);
  Program program;

  if (!vertex_shader.compile(vertex_spurce)) {
    LOG_ERROR("Vertex shader compilation failed: {}",
              vertex_shader.get_info_log());
  }

  if (!fragment_shader.compile(fragment_source)) {
    LOG_ERROR("Fragment shader compilation failed: {}",
              fragment_shader.get_info_log());
  }

  program.attach(vertex_shader);
  program.attach(fragment_shader);
  if (!program.link()) {
    LOG_ERROR("Shader program linking failed: {}", program.get_info_log());
  }

  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  while (!glfwWindowShouldClose(window)) {
    // Check if any events have been activated (key pressed, mouse moved etc.)
    // and call corresponding response functions
    glfwPollEvents();

    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a triangle
    program.use();
    glDrawArrays(GL_TRIANGLES, 0, 3);


    // Swap the screen buffers
    glfwSwapBuffers(window);
  }

  // Terminates GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();

  return 0;
}