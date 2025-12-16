#include "paimon/rendering/render_context.h"

#include "paimon/opengl/shader_program.h"

namespace paimon {

ShaderProgram* RenderContext::createShaderProgram(const ShaderSource& source,
                                            const std::vector<ShaderDefine>& defines) {

  return m_shaderProgramCache.get(source, defines);
}

void RenderContext::beginRendering(const RenderingInfo& info) {
  m_insideRenderPass = true;

  // Get or create framebuffer from cache based on attachments
  m_currentFbo = m_framebufferCache.get(info);

  // Bind framebuffer
  m_currentFbo->bind();

  // Set viewport to render area if specified
  if (info.renderAreaExtent.x > 0 && info.renderAreaExtent.y > 0) {
    glViewport(info.renderAreaOffset.x, info.renderAreaOffset.y,
               info.renderAreaExtent.x, info.renderAreaExtent.y);
  }

  // Clear color attachments
  for (size_t i = 0; i < info.colorAttachments.size(); ++i) {
    const auto& attachment = info.colorAttachments[i];
    if (attachment.loadOp == AttachmentLoadOp::Clear) {
      const float clearColor[4] = {
        attachment.clearValue.color.r,
        attachment.clearValue.color.g,
        attachment.clearValue.color.b,
        attachment.clearValue.color.a
      };
      m_currentFbo->clear(GL_COLOR, static_cast<GLint>(i), clearColor);
    }
  }

  // Clear depth attachment
  if (info.depthAttachment.has_value()) {
    const auto& attachment = info.depthAttachment.value();
    if (attachment.loadOp == AttachmentLoadOp::Clear) {
      float depth = attachment.clearValue.depth;
      m_currentFbo->clear(GL_DEPTH, 0, &depth);
    }
  }

  // Clear stencil attachment
  if (info.stencilAttachment.has_value()) {
    const auto& attachment = info.stencilAttachment.value();
    if (attachment.loadOp == AttachmentLoadOp::Clear) {
      GLint stencilValue = static_cast<GLint>(attachment.clearValue.stencil);
      m_currentFbo->clear(GL_STENCIL, 0, &stencilValue);
    }
  }
}

void RenderContext::endRendering() {
  m_insideRenderPass = false;
  m_currentFbo = nullptr;
  m_currentVao = nullptr;
  // Unbind framebuffer
  Framebuffer::unbind();
  VertexArray::unbind();
}

void RenderContext::beginSwapchainRendering(const SwapchainRenderingInfo& info) {
  m_insideRenderPass = true;
  
  // Get default framebuffer from cache
  m_currentFbo = m_framebufferCache.get(info);

  // Bind default framebuffer
  m_currentFbo->bind();

  // Set viewport to render area if specified
  if (info.renderAreaExtent.x > 0 && info.renderAreaExtent.y > 0) {
    glViewport(info.renderAreaOffset.x, info.renderAreaOffset.y,
               info.renderAreaExtent.x, info.renderAreaExtent.y);
  }

  // Clear color
  const float clearColor[4] = {
    info.clearColor.color.r,
    info.clearColor.color.g,
    info.clearColor.color.b,
    info.clearColor.color.a
  };
  m_currentFbo->clear(GL_COLOR, 0, clearColor);

  // Clear depth
  m_currentFbo->clear(GL_DEPTH, 0, &info.clearDepth);

  // Clear stencil
  GLint stencilValue = static_cast<GLint>(info.clearStencil);
  m_currentFbo->clear(GL_STENCIL, 0, &stencilValue);
}

void RenderContext::endSwapchainRendering() {
  m_insideRenderPass = false;
  m_currentFbo = nullptr;
}

void RenderContext::bindPipeline(const GraphicsPipeline& pipeline) {
  // Bind the program pipeline
  pipeline.bind();

  m_currentPipelineState.apply(pipeline.getState());
  // Vertex Input State is handled via Vertex Array Objects
  auto nextVao = m_vertexArrayCache.get(pipeline.getState().vertexInput);
  if (nextVao != m_currentVao) {
    nextVao->bind();
    m_currentVao = nextVao;
  }
}

void RenderContext::setViewport(float x, float y, float width, float height) {
  glViewport(static_cast<GLint>(x), static_cast<GLint>(y),
             static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

void RenderContext::setViewport(uint32_t index, float x, float y, float width, float height) {
  glViewportIndexedf(index, x, y, width, height);
}

void RenderContext::setScissor(int x, int y, int width, int height) {
  glScissor(x, y, width, height);
}

void RenderContext::setScissor(uint32_t index, int x, int y, int width, int height) {
  glScissorIndexed(index, x, y, width, height);
}

void RenderContext::bindVertexBuffer(uint32_t binding, const Buffer& buffer,
                                    GLintptr offset, GLsizei stride) {
  m_currentVao->set_vertex_buffer(binding, buffer, offset, stride);
}

void RenderContext::bindIndexBuffer(const Buffer& buffer, GLenum indexType) {
  m_currentVao->set_element_buffer(buffer);

  m_currentIndexType = indexType;
}

void RenderContext::bindUniformBuffer(uint32_t binding, const Buffer& buffer,
                                      GLintptr offset, GLsizeiptr size) {
  buffer.bind_range(GL_UNIFORM_BUFFER, binding, offset, size);
}

void RenderContext::bindStorageBuffer(uint32_t binding, const Buffer& buffer,
                                      GLintptr offset, GLsizeiptr size) {
  buffer.bind_range(GL_SHADER_STORAGE_BUFFER, binding, offset, size);
}

void RenderContext::bindTexture(uint32_t unit, const Texture& texture,
                                 const Sampler& sampler) {
  texture.bind(unit);
  sampler.bind(unit);
}

void RenderContext::bindImage(uint32_t unit, const Texture& texture,
                                GLenum access, GLenum format, uint32_t level,
                                GLboolean layered, uint32_t layer) {
  texture.bind(unit, access, format, level, layered, layer);
}

// Non-indexed draw commands
void RenderContext::drawArrays(GLint first, GLsizei count) {
  glDrawArrays(m_currentPipelineState.inputAssembly.topology, first, count);
}

void RenderContext::drawArraysInstanced(GLint first, GLsizei count,
                                       GLsizei instanceCount) {
  glDrawArraysInstanced(m_currentPipelineState.inputAssembly.topology, 
                       first, count, instanceCount);
}

void RenderContext::drawArraysInstancedBaseInstance(GLint first, GLsizei count,
                                                   GLsizei instanceCount,
                                                   GLuint baseInstance) {
  glDrawArraysInstancedBaseInstance(m_currentPipelineState.inputAssembly.topology,
                                   first, count, instanceCount, baseInstance);
}

void RenderContext::drawArraysIndirect(const void* indirect) {
  glDrawArraysIndirect(m_currentPipelineState.inputAssembly.topology, indirect);
}

void RenderContext::multiDrawArrays(const GLint* first, const GLsizei* count,
                                   GLsizei drawCount) {
  glMultiDrawArrays(m_currentPipelineState.inputAssembly.topology, 
                   first, count, drawCount);
}

void RenderContext::multiDrawArraysIndirect(const void* indirect,
                                           GLsizei drawCount, GLsizei stride) {
  glMultiDrawArraysIndirect(m_currentPipelineState.inputAssembly.topology,
                           indirect, drawCount, stride);
}

// Indexed draw commands
void RenderContext::drawElements(GLsizei count, const void* indices) {
  glDrawElements(m_currentPipelineState.inputAssembly.topology, 
                count, m_currentIndexType, indices);
}

void RenderContext::drawElementsBaseVertex(GLsizei count, const void* indices,
                                          GLint baseVertex) {
  glDrawElementsBaseVertex(m_currentPipelineState.inputAssembly.topology,
                          count, m_currentIndexType, indices, baseVertex);
}

void RenderContext::drawElementsInstanced(GLsizei count, const void* indices,
                                         GLsizei instanceCount) {
  glDrawElementsInstanced(m_currentPipelineState.inputAssembly.topology,
                         count, m_currentIndexType, indices, instanceCount);
}

void RenderContext::drawElementsInstancedBaseInstance(GLsizei count, const void* indices,
                                                     GLsizei instanceCount,
                                                     GLuint baseInstance) {
  glDrawElementsInstancedBaseInstance(m_currentPipelineState.inputAssembly.topology,
                                     count, m_currentIndexType, indices,
                                     instanceCount, baseInstance);
}

void RenderContext::drawElementsInstancedBaseVertex(GLsizei count, const void* indices,
                                                   GLsizei instanceCount, GLint baseVertex) {
  glDrawElementsInstancedBaseVertex(m_currentPipelineState.inputAssembly.topology,
                                   count, m_currentIndexType, indices,
                                   instanceCount, baseVertex);
}

void RenderContext::drawElementsInstancedBaseVertexBaseInstance(
    GLsizei count, const void* indices, GLsizei instanceCount, 
    GLint baseVertex, GLuint baseInstance) {
  glDrawElementsInstancedBaseVertexBaseInstance(
      m_currentPipelineState.inputAssembly.topology, count, m_currentIndexType, indices,
      instanceCount, baseVertex, baseInstance);
}

void RenderContext::drawElementsIndirect(const void* indirect) {
  glDrawElementsIndirect(m_currentPipelineState.inputAssembly.topology,
                        m_currentIndexType, indirect);
}

void RenderContext::drawRangeElements(GLuint start, GLuint end, GLsizei count,
                                     const void* indices) {
  glDrawRangeElements(m_currentPipelineState.inputAssembly.topology,
                     start, end, count, m_currentIndexType, indices);
}

void RenderContext::drawRangeElementsBaseVertex(GLuint start, GLuint end, GLsizei count,
                                               const void* indices, GLint baseVertex) {
  glDrawRangeElementsBaseVertex(m_currentPipelineState.inputAssembly.topology,
                               start, end, count, m_currentIndexType, indices, baseVertex);
}

void RenderContext::multiDrawElements(const GLsizei* count, const void* const* indices,
                                     GLsizei drawCount) {
  glMultiDrawElements(m_currentPipelineState.inputAssembly.topology,
                     count, m_currentIndexType, indices, drawCount);
}

void RenderContext::multiDrawElementsBaseVertex(const GLsizei* count,
                                               const void* const* indices,
                                               GLsizei drawCount, const GLint* baseVertex) {
  glMultiDrawElementsBaseVertex(m_currentPipelineState.inputAssembly.topology,
                               count, m_currentIndexType, indices, drawCount, baseVertex);
}

void RenderContext::multiDrawElementsIndirect(const void* indirect, GLsizei drawCount,
                                             GLsizei stride) {
  glMultiDrawElementsIndirect(m_currentPipelineState.inputAssembly.topology,
                             m_currentIndexType, indirect, drawCount, stride);
}

} // namespace paimon
