#include "paimon/core/log_system.h"
#include "paimon/core/resource/resource_manager.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/platform/window.h"
#include "paimon/rendering/shader_source.h"

#include <glm/glm.hpp>
#include <vector>

using namespace paimon;

int main() {
  LogSystem::init();

  auto& resMgr = ResourceManager::getInstance();
  resMgr.initialize(std::filesystem::current_path().parent_path().parent_path());

  LOG_INFO("=== Resource Manager Demo ===");
  LOG_INFO("Asset root: {}", resMgr.getAssetRoot().string());
  LOG_INFO("Shader path: {}", resMgr.getShaderPath().string());

  auto window = Window::create(WindowConfig{
      .title = "Resource Manager Demo",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 6,
              .profile = ContextProfile::Core,
              .debug = false,
          },
      .width = 800,
      .height = 600,
      .resizable = false,
      .visible = true,
      .vsync = false,
  });

  LOG_INFO("\n=== Loading shaders using ShaderSource ===");
  ShaderSource vertSource("vertex/basic.vert");
  ShaderSource fragSource("fragment/basic.frag");

  Shader vertShader(GL_VERTEX_SHADER);
  Shader fragShader(GL_FRAGMENT_SHADER);
  Program program;

  if (!vertShader.compile(vertSource.getSource())) {
    LOG_ERROR("Vertex shader compilation failed: {}",
              vertShader.get_info_log());
    return -1;
  }
  LOG_INFO("Vertex shader compiled successfully");

  if (!fragShader.compile(fragSource.getSource())) {
    LOG_ERROR("Fragment shader compilation failed: {}",
              fragShader.get_info_log());
    return -1;
  }
  LOG_INFO("Fragment shader compiled successfully");

  program.attach(vertShader);
  program.attach(fragShader);
  if (!program.link()) {
    LOG_ERROR("Shader program linking failed: {}", program.get_info_log());
    return -1;
  }
  LOG_INFO("Program linked successfully");

  LOG_INFO("\n=== Manually resolving resource paths ===");
  
  auto vertPath = resMgr.resolve("vertex/basic.vert", 
                                 ResourceManager::ResourceType::Shader);
  if (!vertPath.empty()) {
    LOG_INFO("Resolved vertex shader: {}", vertPath.string());
  }

  if (resMgr.exists("common/common.glsl", ResourceManager::ResourceType::Shader)) {
    LOG_INFO("Common shader library found");
  }

  std::vector<glm::vec3> positions = {
      {-0.5f, -0.5f, 0.0f}, // Bottom left
      {0.5f, -0.5f, 0.0f},  // Bottom right
      {-0.5f, 0.5f, 0.0f},  // Top left
      {0.5f, 0.5f, 0.0f}    // Top right
  };

  std::vector<glm::vec2> texcoords = {
      {0.0f, 0.0f}, // Bottom left
      {1.0f, 0.0f}, // Bottom right
      {0.0f, 1.0f}, // Top left
      {1.0f, 1.0f}  // Top right
  };

  std::vector<unsigned int> indices = {0, 1, 2, 1, 3, 2};

  Buffer pos_vbo;
  pos_vbo.set_storage(positions.size() * sizeof(glm::vec3), positions.data());

  Buffer tex_vbo;
  tex_vbo.set_storage(texcoords.size() * sizeof(glm::vec2), texcoords.data());

  Buffer ebo;
  ebo.set_storage(indices.size() * sizeof(unsigned int), indices.data());

  VertexArray vao;
  auto &binding_pos = vao.get_binding(0);
  binding_pos.bind_buffer(pos_vbo, 0, sizeof(glm::vec3));

  auto &binding_tex = vao.get_binding(1);
  binding_tex.bind_buffer(tex_vbo, 0, sizeof(glm::vec2));

  auto &attribute_pos = vao.get_attribute(0);
  attribute_pos.set_format(3, GL_FLOAT, GL_FALSE, 0);
  attribute_pos.bind(binding_pos);
  attribute_pos.enable();

  auto &attribute_tex = vao.get_attribute(1);
  attribute_tex.set_format(2, GL_FLOAT, GL_FALSE, 0);
  attribute_tex.bind(binding_tex);
  attribute_tex.enable();

  vao.set_element_buffer(ebo);

  std::vector<glm::u8vec4> image_data(100);
  for (auto &pixel : image_data) {
    pixel = glm::u8vec4(255, 0, 0, 255); // Red color
  }

  Texture texture(GL_TEXTURE_2D);
  texture.set_storage_2d(1, GL_RGBA8, 10, 10);
  texture.set_sub_image_2d(0, 0, 0, 10, 10, GL_RGBA, GL_UNSIGNED_BYTE,
                           image_data.data());
  texture.generate_mipmap();

  texture.set(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture.set(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture.set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  texture.set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  LOG_INFO("\n=== Starting render loop ===");

  while (!window->shouldClose()) {
    window->pollEvents();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    program.use();
    texture.bind(0);
    vao.bind();

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                   GL_UNSIGNED_INT, nullptr);

    window->swapBuffers();
  }

  LOG_INFO("Demo completed successfully!");
  return 0;
}
