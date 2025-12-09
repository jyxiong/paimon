#pragma once

#include <glad/gl.h>

#include "paimon/opengl/buffer.h"
#include "paimon/opengl/framebuffer.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/state.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/rendering/framebuffer_cache.h"
#include "paimon/rendering/graphics_pipeline.h"
#include "paimon/rendering/rendering_info.h"
#include "paimon/rendering/vertex_array_cache.h"

namespace paimon {

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

  // Set viewport
  void setViewport(float x, float y, float width, float height);
  void setViewport(uint32_t index, float x, float y, float width, float height);

  // Set scissor
  void setScissor(int x, int y, int width, int height);
  void setScissor(uint32_t index, int x, int y, int width, int height);

  // Bind vertex buffers
  void bindVertexBuffer(uint32_t binding, const Buffer& buffer, 
                       GLintptr offset, GLsizei stride);

  // Bind index buffer
  void bindIndexBuffer(const Buffer& buffer, GLenum indexType);

  // Bind Uniform Buffer
  void bindUniformBuffer(uint32_t binding, const Buffer& buffer,
                         GLintptr offset, GLsizeiptr size);

  // Bind Shader Storage Buffer
  void bindStorageBuffer(uint32_t binding, const Buffer& buffer,
                         GLintptr offset, GLsizeiptr size);

  // Bind Texture & Sampler
  void bindTexture(uint32_t unit, const Texture& texture, 
                   const Sampler& sampler);

  // Bind Image
  void bindImage(uint32_t unit, const Texture& texture,
                 GLenum access, GLenum format, uint32_t level = 0,
                 GLboolean layered = GL_FALSE, uint32_t layer = 0);

  // Non-indexed draw commands
  void drawArrays(GLint first, GLsizei count);
  
  void drawArraysInstanced(GLint first, GLsizei count, GLsizei instanceCount);
  
  void drawArraysInstancedBaseInstance(GLint first, GLsizei count,
                                      GLsizei instanceCount, GLuint baseInstance);
  
  void drawArraysIndirect(const void* indirect);
  
  void multiDrawArrays(const GLint* first, const GLsizei* count, GLsizei drawCount);
  
  void multiDrawArraysIndirect(const void* indirect, GLsizei drawCount, GLsizei stride);

  // Indexed draw commands
  void drawElements(GLsizei count, const void* indices);
  
  void drawElementsBaseVertex(GLsizei count, const void* indices, GLint baseVertex);
  
  void drawElementsInstanced(GLsizei count, const void* indices, GLsizei instanceCount);
  
  void drawElementsInstancedBaseInstance(GLsizei count, const void* indices,
                                        GLsizei instanceCount, GLuint baseInstance);
  
  void drawElementsInstancedBaseVertex(GLsizei count, const void* indices,
                                      GLsizei instanceCount, GLint baseVertex);
  
  void drawElementsInstancedBaseVertexBaseInstance(GLsizei count, const void* indices,
                                                  GLsizei instanceCount, GLint baseVertex,
                                                  GLuint baseInstance);
  
  void drawElementsIndirect(const void* indirect);
  
  void drawRangeElements(GLuint start, GLuint end, GLsizei count, const void* indices);
  
  void drawRangeElementsBaseVertex(GLuint start, GLuint end, GLsizei count,
                                  const void* indices, GLint baseVertex);
  
  void multiDrawElements(const GLsizei* count, const void* const* indices, GLsizei drawCount);
  
  void multiDrawElementsBaseVertex(const GLsizei* count, const void* const* indices,
                                  GLsizei drawCount, const GLint* baseVertex);
  
  void multiDrawElementsIndirect(const void* indirect, GLsizei drawCount, GLsizei stride);

private:
  bool m_insideRenderPass = false;

  Framebuffer* m_currentFbo = nullptr;
  VertexArray* m_currentVao = nullptr;

  GLenum m_currentIndexType = GL_UNSIGNED_INT;

  PipelineState m_currentPipelineState;

  FramebufferCache m_framebufferCache;
  VertexArrayCache m_vertexArrayCache;
};

} // namespace paimon
