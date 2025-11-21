#include "paimon/rendering/render_context.h"

#include "paimon/opengl/buffer.h"
#include "paimon/opengl/framebuffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/vertex_array.h"

namespace paimon {

void RenderContext::beginRendering(const RenderingInfo& info) {
  m_insideRenderPass = true;
  m_currentRenderingInfo = info;

  // Bind framebuffer
  if (info.framebuffer) {
    info.framebuffer->bind();
  } else {
    Framebuffer::unbind(); // Bind default framebuffer
  }

  // Set viewport to render area if specified
  if (info.renderAreaExtent.x > 0 && info.renderAreaExtent.y > 0) {
    glViewport(info.renderAreaOffset.x, info.renderAreaOffset.y,
               info.renderAreaExtent.x, info.renderAreaExtent.y);
  }

  // Apply clear operations
  applyClearOperations(info);
}

void RenderContext::endRendering() {
  m_insideRenderPass = false;
  // Unbind framebuffer
  Framebuffer::unbind();
}

void RenderContext::bindPipeline(const GraphicsPipeline& pipeline) {
  // Bind the program pipeline
  pipeline.bind();

  m_currentPipelineState.apply(pipeline.getState());
}

void RenderContext::bindProgram(const Program& program) {
  program.use();
}

void RenderContext::bindVertexArray(const VertexArray& vao) {
  vao.bind();
}

void RenderContext::bindVertexBuffer(uint32_t binding, const Buffer& buffer,
                                    GLintptr offset, GLsizei stride) {
  buffer.bind(GL_ARRAY_BUFFER);
  glBindVertexBuffer(binding, static_cast<GLuint>(buffer.get_name()), offset, stride);
}

void RenderContext::bindIndexBuffer(const Buffer& buffer) {
  buffer.bind(GL_ELEMENT_ARRAY_BUFFER);
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

void RenderContext::draw(uint32_t vertexCount, uint32_t instanceCount,
                        uint32_t firstVertex, uint32_t firstInstance) {
  if (instanceCount == 1 && firstInstance == 0) {
    glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
  } else {
    glDrawArraysInstancedBaseInstance(GL_TRIANGLES, firstVertex, vertexCount,
                                     instanceCount, firstInstance);
  }
}

void RenderContext::drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                                uint32_t firstIndex, int32_t vertexOffset,
                                uint32_t firstInstance) {
  const void* indices = reinterpret_cast<const void*>(
      static_cast<uintptr_t>(firstIndex * sizeof(uint32_t)));

  if (instanceCount == 1 && firstInstance == 0 && vertexOffset == 0) {
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indices);
  } else {
    glDrawElementsInstancedBaseVertexBaseInstance(
        GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, indices,
        instanceCount, vertexOffset, firstInstance);
  }
}

void RenderContext::clearColorAttachment(uint32_t attachmentIndex, 
                                         const float* clearColor) {
  if (m_currentRenderingInfo.framebuffer) {
    m_currentRenderingInfo.framebuffer->clear(GL_COLOR, attachmentIndex, clearColor);
  } else {
    glClearBufferfv(GL_COLOR, attachmentIndex, clearColor);
  }
}

void RenderContext::clearDepthAttachment(float depth) {
  glClearBufferfv(GL_DEPTH, 0, &depth);
}

void RenderContext::clearStencilAttachment(uint32_t stencil) {
  GLint stencilValue = static_cast<GLint>(stencil);
  glClearBufferiv(GL_STENCIL, 0, &stencilValue);
}

void RenderContext::clearDepthStencilAttachment(float depth, uint32_t stencil) {
  glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, static_cast<GLint>(stencil));
}

void RenderContext::applyClearOperations(const RenderingInfo& info) {
  // Clear color attachments
  for (size_t i = 0; i < info.colorAttachments.size(); ++i) {
    if (info.colorAttachments[i].has_value()) {
      const auto& attachment = info.colorAttachments[i].value();
      if (attachment.loadOp == AttachmentLoadOp::Clear) {
        const float clearColor[4] = {
          attachment.clearValue.color.r,
          attachment.clearValue.color.g,
          attachment.clearValue.color.b,
          attachment.clearValue.color.a
        };
        clearColorAttachment(static_cast<uint32_t>(i), clearColor);
      }
    }
  }

  // Clear depth attachment
  if (info.depthAttachment.has_value()) {
    const auto& attachment = info.depthAttachment.value();
    if (attachment.loadOp == AttachmentLoadOp::Clear) {
      clearDepthAttachment(attachment.clearValue.depthStencil.depth);
    }
  }

  // Clear stencil attachment
  if (info.stencilAttachment.has_value()) {
    const auto& attachment = info.stencilAttachment.value();
    if (attachment.loadOp == AttachmentLoadOp::Clear) {
      clearStencilAttachment(attachment.clearValue.depthStencil.stencil);
    }
  }
}

} // namespace paimon
