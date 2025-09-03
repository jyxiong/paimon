#include "paimon/opengl/state_tracker.h"

using namespace paimon;

void ColorBlendTracker::apply(const ColorBlendState &state) {
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

  if (memcmp(m_cache.blendConstants, state.blendConstants, sizeof(state.blendConstants)) != 0) {
    memcpy(m_cache.blendConstants, state.blendConstants, sizeof(state.blendConstants));
    glBlendColor(state.blendConstants[0], state.blendConstants[1], state.blendConstants[2], state.blendConstants[3]);
  }

  if (m_cache.attachments.size() != state.attachments.size()) {
    m_cache.attachments.resize(state.attachments.size());
  }

  for (size_t i = 0; i < state.attachments.size(); ++i) {
    const auto &src = state.attachments[i];
    auto &dst = m_cache.attachments[i];

    if (dst.enabled != src.enabled) {
      dst.enabled = src.enabled;
      if (src.enabled) {
        glEnablei(GL_BLEND, static_cast<GLuint>(i));
      } else {
        glDisablei(GL_BLEND, static_cast<GLuint>(i));
      }
    }

    if (!src.enabled) {
      continue;
    }

    if (dst.srcRGBFactor != src.srcRGBFactor || dst.dstRGBFactor != src.dstRGBFactor ||
        dst.rgbBlendOp != src.rgbBlendOp || dst.srcAlphaFactor != src.srcAlphaFactor ||
        dst.dstAlphaFactor != src.dstAlphaFactor || dst.alphaBlendOp != src.alphaBlendOp) {
      dst.srcRGBFactor = src.srcRGBFactor;
      dst.dstRGBFactor = src.dstRGBFactor;
      dst.rgbBlendOp = src.rgbBlendOp;
      dst.srcAlphaFactor = src.srcAlphaFactor;
      dst.dstAlphaFactor = src.dstAlphaFactor;
      dst.alphaBlendOp = src.alphaBlendOp;

      glBlendFuncSeparatei(static_cast<GLuint>(i), src.srcRGBFactor, src.dstRGBFactor,
                           src.srcAlphaFactor, src.dstAlphaFactor);
      glBlendEquationSeparatei(static_cast<GLuint>(i), src.rgbBlendOp, src.alphaBlendOp);
    }
  }
}

void DepthTracker::apply(const DepthState &state) {
  if (m_cache.depthTestEnable != state.depthTestEnable) {
    m_cache.depthTestEnable = state.depthTestEnable;
    if (state.depthTestEnable) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  if (m_cache.depthWriteEnable != state.depthWriteEnable) {
    m_cache.depthWriteEnable = state.depthWriteEnable;
    glDepthMask(state.depthWriteEnable ? GL_TRUE : GL_FALSE);
  }

  if (m_cache.depthFunc != state.depthFunc) {
    m_cache.depthFunc = state.depthFunc;
    glDepthFunc(state.depthFunc);
  }
}

void InputAssembllyTracker::apply(const InputAssemblyState &state) {
  if (m_cache.primitiveRestartEnable != state.primitiveRestartEnable) {
    m_cache.primitiveRestartEnable = state.primitiveRestartEnable;
    if (state.primitiveRestartEnable) {
      glEnable(GL_PRIMITIVE_RESTART);
    } else {
      glDisable(GL_PRIMITIVE_RESTART);
    }
  }

  if (m_cache.primitiveRestartIndex != state.primitiveRestartIndex) {
    m_cache.primitiveRestartIndex = state.primitiveRestartIndex;
    glPrimitiveRestartIndex(state.primitiveRestartIndex);
  }

  if (m_cache.primitiveTopology != state.primitiveTopology) {
    m_cache.primitiveTopology = state.primitiveTopology;
    // Note: OpenGL does not have a direct equivalent to Vulkan's primitive topology setting.
    // The primitive topology is specified when drawing (e.g., glDrawArrays or glDrawElements).
    // Therefore, we do not set it here.
  }
}

void MultisampleTracker::apply(const MultisampleState &state) {
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

  if (m_cache.sampleMask != state.sampleMask) {
    m_cache.sampleMask = state.sampleMask;
    glSampleMaski(0, state.sampleMask); // OpenGL supports multiple sample masks, we use the first one here.
  }

  if (m_cache.alphaToCoverageEnable != state.alphaToCoverageEnable) {
    m_cache.alphaToCoverageEnable = state.alphaToCoverageEnable;
    if (state.alphaToCoverageEnable) {
      glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    } else {
      glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
  }

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
  if (m_cache.depthClampEnable != state.depthClampEnable) {
    m_cache.depthClampEnable = state.depthClampEnable;
    if (state.depthClampEnable) {
      glEnable(GL_DEPTH_CLAMP);
    } else {
      glDisable(GL_DEPTH_CLAMP);
    }
  }

  if (m_cache.polygonMode != state.polygonMode) {
    m_cache.polygonMode = state.polygonMode;
    glPolygonMode(GL_FRONT_AND_BACK, state.polygonMode);
  }

  if (m_cache.cullMode != state.cullMode) {
    m_cache.cullMode = state.cullMode;
    if (state.cullMode == GL_NONE) {
      glDisable(GL_CULL_FACE);
    } else {
      glEnable(GL_CULL_FACE);
      glCullFace(state.cullMode);
    }
  }

  if (m_cache.frontFace != state.frontFace) {
    m_cache.frontFace = state.frontFace;
    glFrontFace(state.frontFace);
  }

  if (m_cache.depthBiasEnable != state.depthBiasEnable) {
    m_cache.depthBiasEnable = state.depthBiasEnable;
    if (state.depthBiasEnable) {
      glEnable(GL_POLYGON_OFFSET_FILL);
    } else {
      glDisable(GL_POLYGON_OFFSET_FILL);
    }
  }

  if (m_cache.depthBiasConstantFactor != state.depthBiasConstantFactor ||
      m_cache.depthBiasSlopeFactor != state.depthBiasSlopeFactor) {
    m_cache.depthBiasConstantFactor = state.depthBiasConstantFactor;
    m_cache.depthBiasSlopeFactor = state.depthBiasSlopeFactor;
    glPolygonOffset(state.depthBiasSlopeFactor, state.depthBiasConstantFactor);
  }

  if (m_cache.lineWidth != state.lineWidth) {
    m_cache.lineWidth = state.lineWidth;
    glLineWidth(state.lineWidth);
  }

  if (m_cache.pointSize != state.pointSize) {
    m_cache.pointSize = state.pointSize;
    glPointSize(state.pointSize);
  }
}

void ScissorTracker::apply(const ScissorState &state) {
  if (m_cache.scissorTestEnable != state.scissorTestEnable) {
    m_cache.scissorTestEnable = state.scissorTestEnable;
    if (state.scissorTestEnable) {
      glEnable(GL_SCISSOR_TEST);
    } else {
      glDisable(GL_SCISSOR_TEST);
    }
  }

  // if (m_cache.scissors != state.scissors) {
  //   m_cache.scissors = state.scissors;
  //   for (size_t i = 0; i < state.scissors.size(); ++i) {
  //     const auto &rect = state.scissors[i];
  //     glScissor(rect.x, rect.y, rect.width, rect.height);
  //   }
  // }
}

void TessellationTracker::apply(const TessellationState &state) {
  if (m_cache.patchControlPoints != state.patchControlPoints) {
    m_cache.patchControlPoints = state.patchControlPoints;
    glPatchParameteri(GL_PATCH_VERTICES, state.patchControlPoints);
  }
}

void VertexInputTracker::apply(const VertexInputState &state) {
  // if (m_cache.bindings != state.bindings) {
  //   m_cache.bindings = state.bindings;
  //   for (const auto &binding : state.bindings) {
  //     glBindVertexBuffer(binding.binding, binding.buffer, binding.offset, binding.stride);
  //   }
  // }

  // if (m_cache.attributes != state.attributes) {
  //   m_cache.attributes = state.attributes;
  //   for (const auto &attr : state.attributes) {
  //     glEnableVertexAttribArray(attr.location);
  //     glVertexAttribFormat(attr.location, attr.size, attr.type, attr.normalized ? GL_TRUE : GL_FALSE, attr.offset);
  //     glVertexAttribBinding(attr.location, attr.binding);
  //   }
  // }
}