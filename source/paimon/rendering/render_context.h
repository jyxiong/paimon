#pragma once

#include <glad/gl.h>

#include "paimon/opengl/state.h"
#include "paimon/rendering/framebuffer_cache.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/rendering_info.h"
#include "paimon/rendering/vertex_array_cache.h"

namespace paimon {

class Program;
class VertexArray;
class Buffer;

// RenderContext for Vulkan-style rendering commands
class RenderContext {
public:
  RenderContext() = default;
  ~RenderContext() = default;

  // Delete copy constructor and assignment
  RenderContext(const RenderContext&) = delete;
  RenderContext& operator=(const RenderContext&) = delete;

  // Begin rendering pass
  void beginRendering(const RenderingInfo& info);

  // End rendering pass
  void endRendering();

  // Begin rendering to swapchain (default framebuffer)
  void beginSwapchainRendering(const SwapchainRenderingInfo& info);

  // End rendering to swapchain
  void endSwapchainRendering();

  // Bind graphics pipeline (applies all pipeline states)
  void bindPipeline(const GraphicsPipeline& pipeline);

  // Bind shader program
  void bindProgram(const Program& program);

  // Bind vertex buffers
  void bindVertexBuffer(uint32_t binding, const Buffer& buffer, 
                       GLintptr offset, GLsizei stride);

  // Bind index buffer
  void bindIndexBuffer(const Buffer& buffer);

  // Set viewport
  void setViewport(float x, float y, float width, float height);
  void setViewport(uint32_t index, float x, float y, float width, float height);

  // Set scissor
  void setScissor(int x, int y, int width, int height);
  void setScissor(uint32_t index, int x, int y, int width, int height);

  // Draw commands
  void draw(uint32_t vertexCount, uint32_t instanceCount = 1, 
            uint32_t firstVertex = 0, uint32_t firstInstance = 0);

  void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1,
                  uint32_t firstIndex = 0, int32_t vertexOffset = 0,
                  uint32_t firstInstance = 0);

private:
  bool m_insideRenderPass = false;

  Framebuffer* m_currentFbo = nullptr;
  VertexArray* m_currentVao = nullptr;

  PipelineState m_currentPipelineState;

  FramebufferCache m_framebufferCache;
  VertexArrayCache m_vertexArrayCache;
};

} // namespace paimon
