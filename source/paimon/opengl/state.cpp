#include "paimon/opengl/state.h"

using namespace paimon;

ColorBlendTracker::ColorBlendTracker() {
  int maxColorAttachments = 0;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
  m_cache.attachments.resize(static_cast<size_t>(maxColorAttachments));
}

void ColorBlendTracker::apply(const ColorBlendState &state) {
  // Logic operation
  if (m_cache.logicOpEnable != state.logicOpEnable) {
    m_cache.logicOpEnable = state.logicOpEnable;
    if (state.logicOpEnable) {
      glEnable(GL_COLOR_LOGIC_OP);
    } else {
      glDisable(GL_COLOR_LOGIC_OP);
    }
  }

  if (m_cache.logicOp != state.logicOp) {
    m_cache.logicOp = state.logicOp;
    glLogicOp(state.logicOp);
  }

  // Blend constants
  bool constantsChanged = false;
  for (int i = 0; i < 4; ++i) {
    if (m_cache.blendConstants[i] != state.blendConstants[i]) {
      constantsChanged = true;
      break;
    }
  }
  if (constantsChanged) {
    std::copy(std::begin(state.blendConstants), std::end(state.blendConstants),
              std::begin(m_cache.blendConstants));
    glBlendColor(state.blendConstants[0], state.blendConstants[1],
                 state.blendConstants[2], state.blendConstants[3]);
  }

  // Per-attachment blend state
  for (size_t i = 0; i < state.attachments.size(); ++i) {
    const auto &src = state.attachments[i];
    auto &dst = m_cache.attachments[i];

    // Blend enable
    if (dst.blendEnable != src.blendEnable) {
      dst.blendEnable = src.blendEnable;
      if (src.blendEnable) {
        glEnablei(GL_BLEND, static_cast<GLuint>(i));
      } else {
        glDisablei(GL_BLEND, static_cast<GLuint>(i));
      }
    }

    // Blend functions
    if (dst.srcColorBlendFactor != src.srcColorBlendFactor ||
        dst.dstColorBlendFactor != src.dstColorBlendFactor ||
        dst.srcAlphaBlendFactor != src.srcAlphaBlendFactor ||
        dst.dstAlphaBlendFactor != src.dstAlphaBlendFactor) {
      dst.srcColorBlendFactor = src.srcColorBlendFactor;
      dst.dstColorBlendFactor = src.dstColorBlendFactor;
      dst.srcAlphaBlendFactor = src.srcAlphaBlendFactor;
      dst.dstAlphaBlendFactor = src.dstAlphaBlendFactor;
      glBlendFuncSeparatei(static_cast<GLuint>(i), 
                           src.srcColorBlendFactor, src.dstColorBlendFactor,
                           src.srcAlphaBlendFactor, src.dstAlphaBlendFactor);
    }

    // Blend equations
    if (dst.colorBlendOp != src.colorBlendOp ||
        dst.alphaBlendOp != src.alphaBlendOp) {
      dst.colorBlendOp = src.colorBlendOp;
      dst.alphaBlendOp = src.alphaBlendOp;
      glBlendEquationSeparatei(static_cast<GLuint>(i),
                               src.colorBlendOp, src.alphaBlendOp);
    }

    // Color write mask
    bool maskChanged = false;
    for (int j = 0; j < 4; ++j) {
      if (dst.colorWriteMask[j] != src.colorWriteMask[j]) {
        maskChanged = true;
        break;
      }
    }
    if (maskChanged) {
      std::copy(std::begin(src.colorWriteMask), std::end(src.colorWriteMask),
                std::begin(dst.colorWriteMask));
      glColorMaski(static_cast<GLuint>(i), 
                   src.colorWriteMask[0], src.colorWriteMask[1],
                   src.colorWriteMask[2], src.colorWriteMask[3]);
    }
  }
}

void DepthStencilTracker::apply(const DepthStencilState &state) {
  // Depth test
  if (m_cache.depthTestEnable != state.depthTestEnable) {
    m_cache.depthTestEnable = state.depthTestEnable;
    if (state.depthTestEnable) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  // Depth write
  if (m_cache.depthWriteEnable != state.depthWriteEnable) {
    m_cache.depthWriteEnable = state.depthWriteEnable;
    glDepthMask(state.depthWriteEnable ? GL_TRUE : GL_FALSE);
  }

  // Depth compare operation
  if (m_cache.depthCompareOp != state.depthCompareOp) {
    m_cache.depthCompareOp = state.depthCompareOp;
    glDepthFunc(state.depthCompareOp);
  }

  // Stencil test
  if (m_cache.stencilTestEnable != state.stencilTestEnable) {
    m_cache.stencilTestEnable = state.stencilTestEnable;
    if (state.stencilTestEnable) {
      glEnable(GL_STENCIL_TEST);
    } else {
      glDisable(GL_STENCIL_TEST);
    }
  }

  // Front face stencil
  if (m_cache.front != state.front) {
    m_cache.front = state.front;
    glStencilFuncSeparate(GL_FRONT, state.front.compareOp, 
                         state.front.reference, state.front.compareMask);
    glStencilOpSeparate(GL_FRONT, state.front.failOp, 
                       state.front.depthFailOp, state.front.passOp);
    glStencilMaskSeparate(GL_FRONT, state.front.writeMask);
  }

  // Back face stencil
  if (m_cache.back != state.back) {
    m_cache.back = state.back;
    glStencilFuncSeparate(GL_BACK, state.back.compareOp,
                         state.back.reference, state.back.compareMask);
    glStencilOpSeparate(GL_BACK, state.back.failOp,
                       state.back.depthFailOp, state.back.passOp);
    glStencilMaskSeparate(GL_BACK, state.back.writeMask);
  }
}

void InputAssemblyTracker::apply(const InputAssemblyState &state) {
  // Primitive topology (stored but applied in draw call)
  if (m_cache.topology != state.topology) {
    m_cache.topology = state.topology;
  }

  // Primitive restart
  if (m_cache.primitiveRestartEnable != state.primitiveRestartEnable) {
    m_cache.primitiveRestartEnable = state.primitiveRestartEnable;
    if (state.primitiveRestartEnable) {
      glEnable(GL_PRIMITIVE_RESTART);
    } else {
      glDisable(GL_PRIMITIVE_RESTART);
    }
  }
}

MultisampleTracker::MultisampleTracker() {
  int maxSampleMaskWords = 0;
  glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &maxSampleMaskWords);
  m_cache.sampleMask.resize(static_cast<size_t>(maxSampleMaskWords));
}

void MultisampleTracker::apply(const MultisampleState &state) {
  // Sample shading
  if (m_cache.sampleShadingEnable != state.sampleShadingEnable) {
    m_cache.sampleShadingEnable = state.sampleShadingEnable;
    if (state.sampleShadingEnable) {
      glEnable(GL_SAMPLE_SHADING);
    } else {
      glDisable(GL_SAMPLE_SHADING);
    }
  }

  if (m_cache.minSampleShading != state.minSampleShading) {
    m_cache.minSampleShading = state.minSampleShading;
    glMinSampleShading(state.minSampleShading);
  }

  // Sample mask
  for (size_t i = 0; i < state.sampleMask.size(); ++i) {
    if (m_cache.sampleMask[i] != state.sampleMask[i]) {
      m_cache.sampleMask[i] = state.sampleMask[i];
      glSampleMaski(static_cast<GLuint>(i), state.sampleMask[i]);
    }
  }

  // Alpha to coverage
  if (m_cache.alphaToCoverageEnable != state.alphaToCoverageEnable) {
    m_cache.alphaToCoverageEnable = state.alphaToCoverageEnable;
    if (state.alphaToCoverageEnable) {
      glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    } else {
      glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
  }

  // Alpha to one
  if (m_cache.alphaToOneEnable != state.alphaToOneEnable) {
    m_cache.alphaToOneEnable = state.alphaToOneEnable;
    if (state.alphaToOneEnable) {
      glEnable(GL_SAMPLE_ALPHA_TO_ONE);
    } else {
      glDisable(GL_SAMPLE_ALPHA_TO_ONE);
    }
  }
}

void RasterizationTracker::apply(const RasterizationState &state) {
  // Depth clamp
  if (m_cache.depthClampEnable != state.depthClampEnable) {
    m_cache.depthClampEnable = state.depthClampEnable;
    if (state.depthClampEnable) {
      glEnable(GL_DEPTH_CLAMP);
    } else {
      glDisable(GL_DEPTH_CLAMP);
    }
  }

  // Rasterizer discard
  if (m_cache.rasterizerDiscardEnable != state.rasterizerDiscardEnable) {
    m_cache.rasterizerDiscardEnable = state.rasterizerDiscardEnable;
    if (state.rasterizerDiscardEnable) {
      glEnable(GL_RASTERIZER_DISCARD);
    } else {
      glDisable(GL_RASTERIZER_DISCARD);
    }
  }

  // Polygon mode
  if (m_cache.polygonMode != state.polygonMode) {
    m_cache.polygonMode = state.polygonMode;
    glPolygonMode(GL_FRONT_AND_BACK, state.polygonMode);
  }

  // Cull mode
  if (m_cache.cullMode != state.cullMode) {
    m_cache.cullMode = state.cullMode;
    if (state.cullMode == GL_NONE) {
      glDisable(GL_CULL_FACE);
    } else {
      glEnable(GL_CULL_FACE);
      glCullFace(state.cullMode);
    }
  }

  // Front face
  if (m_cache.frontFace != state.frontFace) {
    m_cache.frontFace = state.frontFace;
    glFrontFace(state.frontFace);
  }

  // Depth bias
  if (m_cache.depthBiasEnable != state.depthBiasEnable) {
    m_cache.depthBiasEnable = state.depthBiasEnable;
    if (state.depthBiasEnable) {
      glEnable(GL_POLYGON_OFFSET_FILL);
      glEnable(GL_POLYGON_OFFSET_LINE);
      glEnable(GL_POLYGON_OFFSET_POINT);
    } else {
      glDisable(GL_POLYGON_OFFSET_FILL);
      glDisable(GL_POLYGON_OFFSET_LINE);
      glDisable(GL_POLYGON_OFFSET_POINT);
    }
  }

  if (m_cache.depthBiasConstantFactor != state.depthBiasConstantFactor ||
      m_cache.depthBiasSlopeFactor != state.depthBiasSlopeFactor) {
    m_cache.depthBiasConstantFactor = state.depthBiasConstantFactor;
    m_cache.depthBiasSlopeFactor = state.depthBiasSlopeFactor;
    glPolygonOffset(state.depthBiasSlopeFactor, state.depthBiasConstantFactor);
  }

  // Line width
  if (m_cache.lineWidth != state.lineWidth) {
    m_cache.lineWidth = state.lineWidth;
    glLineWidth(state.lineWidth);
  }

  // Point size
  if (m_cache.pointSize != state.pointSize) {
    m_cache.pointSize = state.pointSize;
    glPointSize(state.pointSize);
  }

  // Program point size
  if (m_cache.programPointSize != state.programPointSize) {
    m_cache.programPointSize = state.programPointSize;
    if (state.programPointSize) {
      glEnable(GL_PROGRAM_POINT_SIZE);
    } else {
      glDisable(GL_PROGRAM_POINT_SIZE);
    }
  }
}

void TessellationTracker::apply(const TessellationState &state) {
  if (m_cache.patchControlPoints != state.patchControlPoints) {
    m_cache.patchControlPoints = state.patchControlPoints;
    glPatchParameteri(GL_PATCH_VERTICES, state.patchControlPoints);
  }
}

void VertexInputTracker::apply(const VertexInputState &state) {
  // Note: This tracker doesn't directly apply OpenGL state
  // Vertex input configuration is typically applied through VertexArray objects
  // This tracker mainly caches the state for reference
  if (m_cache == state) {
    return;
  }
  
  m_cache = state;
  
  // The actual vertex attribute and binding setup would be done via:
  // - glVertexAttribPointer / glVertexAttribFormat
  // - glBindVertexBuffer
  // - glVertexBindingDivisor
  // These are typically managed by the VertexArray class
}

ViewportTracker::ViewportTracker() {
  int maxViewports = 0;
  glGetIntegerv(GL_MAX_VIEWPORTS, &maxViewports);
  m_cache.viewports.resize(static_cast<size_t>(maxViewports));
  m_cache.scissors.resize(static_cast<size_t>(maxViewports));
}

void ViewportTracker::apply(const ViewportState &state) {
  // Apply viewports
  for (size_t i = 0; i < state.viewports.size() && i < m_cache.viewports.size(); ++i) {
    const auto &src = state.viewports[i];
    auto &dst = m_cache.viewports[i];

    if (dst != src) {
      dst = src;
      glViewportIndexedf(static_cast<GLuint>(i), src.x, src.y,
                         src.width, src.height);
      glDepthRangeIndexed(static_cast<GLuint>(i), src.minDepth, src.maxDepth);
    }
  }

  // Apply scissors
  for (size_t i = 0; i < state.scissors.size() && i < m_cache.scissors.size(); ++i) {
    const auto &src = state.scissors[i];
    auto &dst = m_cache.scissors[i];

    if (dst != src) {
      dst = src;
      glScissorIndexed(static_cast<GLuint>(i), src.x, src.y,
                       src.width, src.height);
    }
  }
}