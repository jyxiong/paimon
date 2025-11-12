#include "paimon/opengl/state.h"

using namespace paimon;

ColorBlendTracker::ColorBlendTracker() {
  int maxColorAttachments = 0;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
  m_cache.attachments.resize(static_cast<size_t>(maxColorAttachments));
}

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

  if (m_cache.blendConstants != state.blendConstants) {
    m_cache.blendConstants = state.blendConstants;
    glBlendColor(state.blendConstants.red, state.blendConstants.green,
                 state.blendConstants.blue, state.blendConstants.alpha);
  }

  for (size_t i = 0; i < state.attachments.size(); ++i) {
    const auto &src = state.attachments[i];
    auto &dst = m_cache.attachments[i];

    if (dst.blendEnabled != src.blendEnabled) {
      dst.blendEnabled = src.blendEnabled;
      if (src.blendEnabled) {
        glEnablei(GL_BLEND, static_cast<GLuint>(i));
      } else {
        glDisablei(GL_BLEND, static_cast<GLuint>(i));
      }
    }

    if (dst.blendFactor != src.blendFactor) {
      dst.blendFactor = src.blendFactor;
      glBlendFuncSeparatei(static_cast<GLuint>(i), src.blendFactor.srcRGBFactor,
                           src.blendFactor.dstRGBFactor,
                           src.blendFactor.srcAlphaFactor,
                           src.blendFactor.dstAlphaFactor);
    }

    if (dst.blendEquation != src.blendEquation) {
      dst.blendEquation = src.blendEquation;
      glBlendEquationSeparatei(static_cast<GLuint>(i),
                               src.blendEquation.rgbBlendOp,
                               src.blendEquation.alphaBlendOp);
    }

    if (dst.colorWriteMask != src.colorWriteMask) {
      dst.colorWriteMask = src.colorWriteMask;
      glColorMaski(static_cast<GLuint>(i), src.colorWriteMask.red,
                   src.colorWriteMask.green, src.colorWriteMask.blue,
                   src.colorWriteMask.alpha);
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

  if (m_cache.depthCompareOp != state.depthCompareOp) {
    m_cache.depthCompareOp = state.depthCompareOp;
    glDepthFunc(state.depthCompareOp);
  }
}

void InputAssemblyTracker::apply(const InputAssemblyState &state) {
  if (m_cache.primitiveTopology != state.primitiveTopology) {
    m_cache.primitiveTopology = state.primitiveTopology;
  }

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
}

MultisampleTracker::MultisampleTracker() {
  int maxSampleMaskWords = 0;
  glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &maxSampleMaskWords);
  m_cache.sampleMasks.resize(static_cast<size_t>(maxSampleMaskWords));
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

  for (size_t i = 0; i < state.sampleMasks.size(); ++i) {
    if (m_cache.sampleMasks[i] != state.sampleMasks[i]) {
      m_cache.sampleMasks[i] = state.sampleMasks[i];
      glSampleMaski(static_cast<GLuint>(i), state.sampleMasks[i]);
    }
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

  if (m_cache.rasterizerDiscardEnable != state.rasterizerDiscardEnable) {
    m_cache.rasterizerDiscardEnable = state.rasterizerDiscardEnable;
    if (state.rasterizerDiscardEnable) {
      glEnable(GL_RASTERIZER_DISCARD);
    } else {
      glDisable(GL_RASTERIZER_DISCARD);
    }
  }

  if (m_cache.polygonMode != state.polygonMode) {
    m_cache.polygonMode = state.polygonMode;
    glPolygonMode(GL_FRONT_AND_BACK, state.polygonMode);
  }

  if (m_cache.cullEnable != state.cullEnable) {
    m_cache.cullEnable = state.cullEnable;
    if (state.cullEnable) {
      glEnable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
    }
  }

  if (m_cache.cullMode != state.cullMode) {
    m_cache.cullMode = state.cullMode;
    glCullFace(state.cullMode);
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

  if (m_cache.depthBias != state.depthBias) {
    m_cache.depthBias = state.depthBias;
    glPolygonOffset(state.depthBias.constant, state.depthBias.slope);
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

ScissorTracker::ScissorTracker() {
  int maxScissors = 0;
  glGetIntegerv(GL_MAX_VIEWPORTS, &maxScissors);
  m_cache.resize(static_cast<size_t>(maxScissors));
}

void ScissorTracker::apply(const std::vector<ScissorState> &state) {
  for (size_t i = 0; i < state.size(); ++i) {
    const auto &src = state[i];
    auto &dst = m_cache[i];

    if (dst.enable != src.enable) {
      dst.enable = src.enable;
      if (src.enable) {
        glEnablei(GL_SCISSOR_TEST, static_cast<GLuint>(i));
      } else {
        glDisablei(GL_SCISSOR_TEST, static_cast<GLuint>(i));
      }
    }

    if (dst.scissor != src.scissor) {
      dst.scissor = src.scissor;
      glScissorIndexed(static_cast<GLuint>(i), src.scissor.x, src.scissor.y,
                       src.scissor.width, src.scissor.height);
    }
  }
}

StencilTracker::StencilTracker() {
  m_cache.faceOps[GL_FRONT] = {};
  m_cache.faceOps[GL_BACK] = {};
}

void StencilTracker::apply(const StencilState &state) {
  if (m_cache.enable != state.enable) {
    m_cache.enable = state.enable;
    if (state.enable) {
      glEnable(GL_STENCIL_TEST);
    } else {
      glDisable(GL_STENCIL_TEST);
    }
  }

  for (const auto &[face, stencilOp] : state.faceOps) {
    if (stencilOp.compareOp != m_cache.faceOps[face].compareOp ||
        stencilOp.reference != m_cache.faceOps[face].reference ||
        stencilOp.compareMask != m_cache.faceOps[face].compareMask) {
      m_cache.faceOps[face].compareOp = stencilOp.compareOp;
      m_cache.faceOps[face].reference = stencilOp.reference;
      m_cache.faceOps[face].compareMask = stencilOp.compareMask;
      glStencilFuncSeparate(face, stencilOp.compareOp, stencilOp.reference,
                            stencilOp.compareMask);
    }

    if (stencilOp.failOp != m_cache.faceOps[face].failOp ||
        stencilOp.depthFailOp != m_cache.faceOps[face].depthFailOp ||
        stencilOp.passOp != m_cache.faceOps[face].passOp) {
      m_cache.faceOps[face].failOp = stencilOp.failOp;
      m_cache.faceOps[face].depthFailOp = stencilOp.depthFailOp;
      m_cache.faceOps[face].passOp = stencilOp.passOp;
      glStencilOpSeparate(face, stencilOp.failOp, stencilOp.depthFailOp,
                          stencilOp.passOp);
    }

    if (stencilOp.writeMask != m_cache.faceOps[face].writeMask) {
      m_cache.faceOps[face].writeMask = stencilOp.writeMask;
      glStencilMaskSeparate(face, stencilOp.writeMask);
    }
  }
}

void TessellationTracker::apply(const TessellationState &state) {
  if (m_cache.patchControlPoints != state.patchControlPoints) {
    m_cache.patchControlPoints = state.patchControlPoints;
    glPatchParameteri(GL_PATCH_VERTICES, state.patchControlPoints);
  }
}

ViewportTracker::ViewportTracker() {
  int maxViewports = 0;
  glGetIntegerv(GL_MAX_VIEWPORTS, &maxViewports);
  m_cache.resize(static_cast<size_t>(maxViewports));
}

void ViewportTracker::apply(const std::vector<ViewportState> &state) {

  for (size_t i = 0; i < state.size(); ++i) {
    const auto &src = state[i];
    auto &dst = m_cache[i];

    if (dst.viewport != src.viewport) {
      dst.viewport = src.viewport;
      glViewportIndexedf(static_cast<GLuint>(i), src.viewport.x, src.viewport.y,
                         src.viewport.width, src.viewport.height);
    }

    if (dst.depthRange != src.depthRange) {
      dst.depthRange = src.depthRange;
      glDepthRangeIndexed(static_cast<GLuint>(i), src.depthRange.near,
                          src.depthRange.far);
    }
  }
}