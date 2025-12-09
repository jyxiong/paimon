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

  // Redeclare built-in block required by ARB_separate_shader_objects
  out gl_PerVertex {
    vec4 gl_Position;
  };

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
  
  LOG_INFO("Vertex program info log: {}", vertex_program.get_info_log());

  if (!validate_shader_program(fragment_program, "Fragment")) {
    return 1;
  }
  
  LOG_INFO("Fragment program info log: {}", fragment_program.get_info_log());

  GraphicsPipelineCreateInfo pipelineInfo{
      .shaderStages = {
          {GL_VERTEX_SHADER_BIT, &vertex_program},
          {GL_FRAGMENT_SHADER_BIT, &fragment_program},
      },
      .state = {
        .depthStencil = {
          .depthTestEnable = false,
        },
        .rasterization = {
          .cullMode = GL_NONE,
        },
      },
  };

  auto pipeline = GraphicsPipeline(pipelineInfo);
  
  // Validate pipeline
  if (!pipeline.validate()) {
    LOG_ERROR("Pipeline validation failed!");
    
    // Get pipeline info log
    GLint logLength = 0;
    glGetProgramPipelineiv(pipeline.get_name(), GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      std::string infoLog(logLength, '\0');
      glGetProgramPipelineInfoLog(pipeline.get_name(), logLength, nullptr, infoLog.data());
      LOG_ERROR("Pipeline info log: {}", infoLog);
    }
    return 1;
  }
  
  LOG_INFO("Pipeline created and validated successfully");

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

  // Position attribute (location 0)
  vao.set_vertex_buffer(0, vbo, 0, sizeof(Vertex));
  vao.set_attribute_format(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  vao.set_attribute_binding(0, 0);
  vao.enable_attribute(0);

  // Color attribute (location 1)
  vao.set_vertex_buffer(1, vbo, 0, sizeof(Vertex));
  vao.set_attribute_format(1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
  vao.set_attribute_binding(1, 1);
  vao.enable_attribute(1);

  
  // Configure viewport
  // Viewport viewport = {0, 0, 800, 600, 0.0f, 1.0f};
  // pipeline.viewportState.viewports.push_back(viewport);

  // Create crender context
  RenderContext ctx;

  LOG_INFO("Starting render loop...");
  LOG_INFO("Window size: {}x{}", g_size.x, g_size.y);

  bool firstFrame = true;
  
  // Main render loop
  while (!window->shouldClose()) {
    window->pollEvents();

    // Setup swapchain rendering info
    SwapchainRenderingInfo swapchainInfo;
    swapchainInfo.renderAreaOffset = {0, 0};
    swapchainInfo.renderAreaExtent = {g_size.x, g_size.y};
    swapchainInfo.clearColor = ClearValue::Color(1.0f, 0.0f, 0.0f, 1.0f); // Red background
    swapchainInfo.clearDepth = 1.0f;
    swapchainInfo.clearStencil = 0;

    // === Vulkan-style rendering commands ===
    
    // Begin rendering to swapchain (default framebuffer)
    ctx.beginSwapchainRendering(swapchainInfo);

    // Bind graphics pipeline (sets shader stages + pipeline state)
    ctx.bindPipeline(pipeline);

    // Draw the triangle
    ctx.drawArrays(0, 3);
    
    if (firstFrame) {
      LOG_INFO("First frame rendered");
      
      // Check for OpenGL errors
      GLenum err;
      while ((err = glGetError()) != GL_NO_ERROR) {
        LOG_ERROR("OpenGL error: 0x{:x}", err);
      }
      
      firstFrame = false;
    }

    // End rendering to swapchain
    ctx.endSwapchainRendering();

    // Swap buffers
    window->swapBuffers();
  }

  LOG_INFO("Render loop ended.");
  return 0;
}
