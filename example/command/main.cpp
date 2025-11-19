#include <glm/glm.hpp>

#include "paimon/app/window.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/rendering/command.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/rendering_info.h"

using namespace paimon;

namespace {
// Vertex shader
std::string vertex_source = R"(
  #version 460 core

  layout(location = 0) in vec3 a_position;
  layout(location = 1) in vec3 a_color;

  out vec3 v_color;

  void main()
  {
    gl_Position = vec4(a_position, 1.0f);
    v_color = a_color;
  }
)";

// Fragment shader
std::string fragment_source = R"(
  #version 460 core
  
  in vec3 v_color;
  out vec4 FragColor;
  
  void main() 
  {
    FragColor = vec4(v_color, 1.0f);
  }
  )";

glm::ivec2 g_size = {800, 600};
} // namespace

int main() {
  LogSystem::init();
  
  LOG_INFO("=== Command Buffer Example ===");
  LOG_INFO("This example demonstrates Vulkan-style rendering commands:");
  LOG_INFO("  - cmd.beginRendering(renderingInfo)");
  LOG_INFO("  - cmd.bindPipeline(pipeline)");
  LOG_INFO("  - cmd.bindProgram(program)");
  LOG_INFO("  - cmd.bindVertexArray(vao)");
  LOG_INFO("  - cmd.draw(...)");
  LOG_INFO("  - cmd.endRendering()");
  LOG_INFO("");

  // Create window
  auto window = Window::create(WindowConfig{
      .title = "Command Buffer Example",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 6,
              .profile = ContextProfile::Core,
              .debug = false,
          },
      .width = static_cast<uint32_t>(g_size.x),
      .height = static_cast<uint32_t>(g_size.y),
      .resizable = true,
      .visible = true,
      .vsync = true,
  });

  // Compile shaders
  Shader vertex_shader(GL_VERTEX_SHADER);
  Shader fragment_shader(GL_FRAGMENT_SHADER);
  Program program;

  if (!vertex_shader.compile(vertex_source)) {
    LOG_ERROR("Vertex shader compilation failed: {}",
              vertex_shader.get_info_log());
    return 1;
  }

  if (!fragment_shader.compile(fragment_source)) {
    LOG_ERROR("Fragment shader compilation failed: {}",
              fragment_shader.get_info_log());
    return 1;
  }

  program.attach(vertex_shader);
  program.attach(fragment_shader);
  if (!program.link()) {
    LOG_ERROR("Shader program linking failed: {}", program.get_info_log());
    return 1;
  }

  // Define triangle vertices (position + color)
  struct Vertex {
    float position[3];
    float color[3];
  };

  Vertex vertices[] = {
      {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // bottom left - red
      {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // bottom right - green
      {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}    // top - blue
  };

  // Create Vertex Buffer Object (VBO)
  Buffer vbo;
  vbo.set_storage(sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);
  vbo.bind(GL_ARRAY_BUFFER);

  // Create Vertex Array Object (VAO)
  VertexArray vao;
  vao.bind();

  // Position attribute (location 0)
  auto &binding0 = vao.get_binding(0);
  binding0.bind_buffer(vbo, 0, sizeof(Vertex));

  auto &position_attr = vao.get_attribute(0);
  position_attr.set_format(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  position_attr.bind(binding0);
  position_attr.enable();

  // Color attribute (location 1)
  auto &binding1 = vao.get_binding(1);
  binding1.bind_buffer(vbo, 0, sizeof(Vertex));

  auto &color_attr = vao.get_attribute(1);
  color_attr.set_format(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
  color_attr.bind(binding1);
  color_attr.enable();

  // Create graphics pipeline (similar to Vulkan)
  GraphicsPipeline pipeline;
  
  // Configure depth testing
  pipeline.depthState.depthTestEnable = false;
  
  // Configure rasterization
  pipeline.rasterizationState.cullMode = GL_BACK;
  pipeline.rasterizationState.frontFace = GL_CCW;
  pipeline.rasterizationState.polygonMode = GL_FILL;
  
  // Configure viewport
  ViewportState viewportState;
  viewportState.viewport = {0, 0, 800, 600};
  viewportState.depthRange = {0.0f, 1.0f};
  pipeline.viewportStates.push_back(viewportState);

  // Create command buffer
  Command cmd;

  LOG_INFO("Starting render loop...");

  // Main render loop
  while (!window->shouldClose()) {
    window->pollEvents();

    // Setup rendering info (similar to VkRenderingInfo)
    RenderingInfo renderingInfo;
    renderingInfo.framebuffer = nullptr; // Use default framebuffer
    renderingInfo.renderAreaOffset = {0, 0};
    renderingInfo.renderAreaExtent = {g_size.x, g_size.y};

    // Setup color attachment
    RenderingAttachmentInfo colorAttachment;
    colorAttachment.loadOp = AttachmentLoadOp::Clear;
    colorAttachment.storeOp = AttachmentStoreOp::Store;
    colorAttachment.clearValue = ClearValue::Color(0.2f, 0.3f, 0.3f, 1.0f);
    renderingInfo.colorAttachments[0] = colorAttachment;

    // Update viewport size
    pipeline.viewportStates[0].viewport.width = g_size.x;
    pipeline.viewportStates[0].viewport.height = g_size.y;

    // === Vulkan-style rendering commands ===
    
    // Begin rendering pass
    cmd.beginRendering(renderingInfo);

    // Bind graphics pipeline
    cmd.bindPipeline(pipeline);

    // Bind shader program
    cmd.bindProgram(program);

    // Bind vertex array
    cmd.bindVertexArray(vao);

    // Draw the triangle
    cmd.draw(3); // 3 vertices

    // End rendering pass
    cmd.endRendering();

    // Swap buffers
    window->swapBuffers();
  }

  LOG_INFO("Render loop ended.");
  return 0;
}
