#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "paimon/app/window.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/rendering/shader_preprocessor.h"
#include "paimon/rendering/shader_source.h"

using namespace paimon;

struct Vertex {
  float position[3];
  float color[3];
};

void printSeparator(const std::string &title) {
  LOG_INFO("");
  LOG_INFO("{}", std::string(60, '='));
  LOG_INFO("  {}", title);
  LOG_INFO("{}", std::string(60, '='));
  LOG_INFO("");
}

std::string processShader(
    const std::filesystem::path &shaderPath,
    const std::filesystem::path &commonDir,
    const std::vector<std::pair<std::string, std::string>> &defines = {}) {
  ShaderSource shaderSource(shaderPath);

  // Add defines
  for (const auto &[key, value] : defines) {
    if (value.empty()) {
      shaderSource.define(key);
    } else {
      shaderSource.define(key, value);
    }
  }

  ShaderPreprocessor preprocessor;
  preprocessor.addSearchPath(commonDir);
  preprocessor.addSearchPath(shaderPath.parent_path());

  return preprocessor.process(shaderSource);
}

int main() {
  LogSystem::init();

  printSeparator("Shader Preprocessor with Triangle Rendering");
  LOG_INFO("This example demonstrates:");
  LOG_INFO("  - Real-time triangle rendering");
  LOG_INFO("  - Shader preprocessing with #include and #define");
  LOG_INFO("  - Different shader variations");

  // Setup shader paths
  auto assetShaderDir =
      std::filesystem::current_path().parent_path() / "asset" / "shader";
  auto commonDir = assetShaderDir / "common";
  auto vertexShaderPath = assetShaderDir / "vertex" / "basic.vert";
  auto fragmentShaderPath = assetShaderDir / "fragment" / "color.frag";

  // Check if files exist
  if (!std::filesystem::exists(vertexShaderPath) ||
      !std::filesystem::exists(fragmentShaderPath)) {
    LOG_ERROR("Shader files not found!");
    LOG_ERROR("Vertex shader: {}", vertexShaderPath.string());
    LOG_ERROR("Fragment shader: {}", fragmentShaderPath.string());
    return 1;
  }

  // Create window
  auto window = Window::create(WindowConfig{
      .title = "Shader Preprocessor Example - Triangle",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 6,
              .profile = ContextProfile::Core,
              .debug = false,
          },
      .width = 800,
      .height = 600,
      .resizable = true,
      .visible = true,
      .vsync = true,
  });

  // Define triangle vertices with colors
  Vertex vertices[] = {
      {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // Bottom left - Red
      {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom right - Green
      {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}    // Top - Blue
  };

  // Create VBO and VAO
  Buffer vbo;
  vbo.set_storage(sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);
  vbo.bind(GL_ARRAY_BUFFER);

  VertexArray vao;
  vao.bind();

  auto &binding = vao.get_binding(0);
  binding.bind_buffer(vbo, 0, sizeof(Vertex));

  // Position attribute
  auto &posAttribute = vao.get_attribute(0);
  posAttribute.set_format(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  posAttribute.bind(binding);
  posAttribute.enable();

  // Color attribute
  auto &colorAttribute = vao.get_attribute(1);
  colorAttribute.set_format(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
  colorAttribute.bind(binding);
  colorAttribute.enable();

  // Scenario 1: Basic rendering (no transform, no post-processing)
  printSeparator("Scenario 1: Basic Triangle");
  LOG_INFO("Processing shaders with minimal defines...");

  auto vertexSource1 = processShader(vertexShaderPath, commonDir, {});
  auto fragmentSource1 = processShader(fragmentShaderPath, commonDir, {});

  Shader vertexShader1(GL_VERTEX_SHADER);
  Shader fragmentShader1(GL_FRAGMENT_SHADER);
  Program program1;

  if (!vertexShader1.compile(vertexSource1)) {
    LOG_ERROR("Vertex shader compilation failed: {}",
              vertexShader1.get_info_log());
    return 1;
  }
  if (!fragmentShader1.compile(fragmentSource1)) {
    LOG_ERROR("Fragment shader compilation failed: {}",
              fragmentShader1.get_info_log());
    return 1;
  }
  program1.attach(vertexShader1);
  program1.attach(fragmentShader1);
  if (!program1.link()) {
    LOG_ERROR("Program linking failed: {}", program1.get_info_log());
    return 1;
  }
  LOG_INFO("Basic shader program compiled successfully");

  // Scenario 2: With gamma correction
  printSeparator("Scenario 2: With Gamma Correction");

  auto vertexSource2 = processShader(vertexShaderPath, commonDir, {});
  auto fragmentSource2 = processShader(fragmentShaderPath, commonDir,
                                       {{"ENABLE_GAMMA_CORRECTION", ""}});

  Shader vertexShader2(GL_VERTEX_SHADER);
  Shader fragmentShader2(GL_FRAGMENT_SHADER);
  Program program2;

  if (!vertexShader2.compile(vertexSource2)) {
    LOG_ERROR("Vertex shader compilation failed: {}",
              vertexShader2.get_info_log());
    return 1;
  }
  if (!fragmentShader2.compile(fragmentSource2)) {
    LOG_ERROR("Fragment shader compilation failed: {}",
              fragmentShader2.get_info_log());
    return 1;
  }
  program2.attach(vertexShader2);
  program2.attach(fragmentShader2);
  if (!program2.link()) {
    LOG_ERROR("Program linking failed: {}", program2.get_info_log());
    return 1;
  }
  LOG_INFO("Gamma correction shader program compiled successfully");

  // Scenario 3: With transform and brightness
  printSeparator("Scenario 3: With Transform and Brightness");

  auto vertexSource3 =
      processShader(vertexShaderPath, commonDir, {{"USE_TRANSFORM", ""}});
  auto fragmentSource3 =
      processShader(fragmentShaderPath, commonDir,
                    {{"ENABLE_BRIGHTNESS", ""}, {"BRIGHTNESS_FACTOR", "1.5"}});

  Shader vertexShader3(GL_VERTEX_SHADER);
  Shader fragmentShader3(GL_FRAGMENT_SHADER);
  Program program3;

  if (!vertexShader3.compile(vertexSource3)) {
    LOG_ERROR("Vertex shader compilation failed: {}",
              vertexShader3.get_info_log());
    return 1;
  }
  if (!fragmentShader3.compile(fragmentSource3)) {
    LOG_ERROR("Fragment shader compilation failed: {}",
              fragmentShader3.get_info_log());
    return 1;
  }
  program3.attach(vertexShader3);
  program3.attach(fragmentShader3);
  if (!program3.link()) {
    LOG_ERROR("Program linking failed: {}", program3.get_info_log());
    return 1;
  }
  LOG_INFO("Transform + brightness shader program compiled successfully");

  printSeparator("Rendering");
  LOG_INFO("Auto-cycling through 3 shader scenarios every 2 seconds");
  LOG_INFO("Rendering animated triangles...");

  int currentScenario = 1;
  float time = 0.0f;
  float scenarioTime = 0.0f;
  const float scenarioDuration = 2.0f; // Switch every 2 seconds

  while (!window->shouldClose()) {
    window->pollEvents();

    time += 0.016f; // Approximate 60 FPS
    scenarioTime += 0.016f;

    // Auto-switch scenarios
    if (scenarioTime >= scenarioDuration) {
      scenarioTime = 0.0f;
      currentScenario = (currentScenario % 3) + 1;

      const char *scenarioNames[] = {"Basic", "Gamma Correction",
                                     "Transform + Brightness"};
      LOG_INFO("Switched to Scenario {}: {}", currentScenario,
               scenarioNames[currentScenario - 1]);
    }

    // Clear screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind VAO
    vao.bind();

    // Use appropriate program based on scenario
    switch (currentScenario) {
    case 1:
      program1.use();
      break;
    case 2:
      program2.use();
      break;
    case 3: {
      program3.use();
      // Apply rotation transform
      glm::mat4 transform = glm::mat4(1.0f);
      transform = glm::rotate(transform, time, glm::vec3(0.0f, 0.0f, 1.0f));
      GLint transformLoc =
          glGetUniformLocation(program3.get_name(), "u_transform");
      if (transformLoc != -1) {
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]);
      }
      break;
    }
    }

    // Draw triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    window->swapBuffers();
  }

  printSeparator("Summary");
  LOG_INFO("Successfully demonstrated:");
  LOG_INFO("  Shader preprocessing with #include");
  LOG_INFO("  Runtime shader variations using #define");
  LOG_INFO("  Real-time triangle rendering");
  LOG_INFO("  Multiple shader programs with different features");

  return 0;
}
