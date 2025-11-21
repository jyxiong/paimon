#include <glm/glm.hpp>

#include "paimon/app/window.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/opengl/shader_program.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/render_context.h"
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
  
  LOG_INFO("=== RenderContext + PipelineManager Example ===");
  LOG_INFO("Workflow:");
  LOG_INFO("  - PipelineManager::create(...) builds a separable pipeline");
  LOG_INFO("  - RenderContext drives beginRendering/bind/draw/end");
  LOG_INFO("");

  // Create window
  auto window = Window::create(WindowConfig{
      .title = "RenderContext Buffer Example",
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

  // Compile separable shader programs for the pipeline
  ShaderProgram vertex_program(GL_VERTEX_SHADER, vertex_source);
  ShaderProgram fragment_program(GL_FRAGMENT_SHADER, fragment_source);

  auto validate_shader_program = [](const ShaderProgram &shader,
                                    const char *label) {
    GLint linkStatus = shader.get(GL_LINK_STATUS);
    if (linkStatus != GL_TRUE) {
      LOG_ERROR("{} shader program compilation failed: {}", label,
                shader.get_info_log());
      return false;
    }
    return true;
  };

  if (!validate_shader_program(vertex_program, "Vertex")) {
    return 1;
  }

  if (!validate_shader_program(fragment_program, "Fragment")) {
    return 1;
  }

  GraphicsPipelineCreateInfo pipelineInfo{
      .shaderStages = {
          {GL_VERTEX_SHADER, &vertex_program},
          {GL_FRAGMENT_SHADER, &fragment_program},
      },
      .state = {
      },
  };

  auto pipeline = GraphicsPipeline(pipelineInfo);

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

  
  // Configure viewport
  // Viewport viewport = {0, 0, 800, 600, 0.0f, 1.0f};
  // pipeline.viewportState.viewports.push_back(viewport);

  // Create crender context
  RenderContext ctx;

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

    // === Vulkan-style rendering commands ===
    
    // Begin rendering pass
    ctx.beginRendering(renderingInfo);

    // Bind graphics pipeline (sets shader stages + pipeline state)
    ctx.bindPipeline(pipeline);

    // Set viewport
    // Note: setViewport like Vulkan with dynamic state
    ctx.setViewport(0, 0, g_size.x, g_size.y);

    // Bind vertex array
    ctx.bindVertexArray(vao);

    // Draw the triangle
    ctx.draw(3); // 3 vertices

    // End rendering pass
    ctx.endRendering();

    // Swap buffers
    window->swapBuffers();
  }

  LOG_INFO("Render loop ended.");
  return 0;
}
