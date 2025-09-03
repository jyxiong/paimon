#include "paimon/opengl/state_tracker.h"

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
    glBlendColor(state.blendConstants[0], state.blendConstants[1], state.blendConstants[2], state.blendConstants[3]);
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

    if (dst.blendFactors != src.blendFactors) {
      dst.blendFactors = src.blendFactors;
      glBlendFuncSeparatei(
          static_cast<GLuint>(i), src.blendFactors.srcRGBFactor,
          src.blendFactors.dstRGBFactor, src.blendFactors.srcAlphaFactor,
          src.blendFactors.dstAlphaFactor);
    }

    if (dst.blendOp != src.blendOp) {
      dst.blendOp = src.blendOp;
      glBlendEquationSeparatei(static_cast<GLuint>(i), src.blendOp.rgbBlendOp,
                               src.blendOp.alphaBlendOp);
    }
  }
}

void DepthTracker::apply(const DepthState &state) {
  if (m_cache.depthTest != state.depthTest) {
    m_cache.depthTest = state.depthTest;
    if (state.depthTest) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  if (m_cache.depthWrite != state.depthWrite) {
    m_cache.depthWrite = state.depthWrite;
    glDepthMask(state.depthWrite ? GL_TRUE : GL_FALSE);
  }

  if (m_cache.depthFunc != state.depthFunc) {
    m_cache.depthFunc = state.depthFunc;
    glDepthFunc(state.depthFunc);
  }
}

void InputAssemblyTracker::apply(const InputAssemblyState &state) {
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
  if (m_cache.enable != state.enable) {
    m_cache.enable = state.enable;
    if (state.enable) {
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

ScissorTracker::ScissorTracker() {
  int maxScissors = 0;
  glGetIntegerv(GL_MAX_VIEWPORTS, &maxScissors);
  m_cache.scissors.resize(static_cast<size_t>(maxScissors));
}

void ScissorTracker::apply(const ScissorState &state) {
  for (size_t i = 0; i < state.scissors.size(); ++i) {
    const auto &src = state.scissors[i];
    auto &dst = m_cache.scissors[i];
    if (dst != src) {
      dst = src;
      if (src.enable) {
        glEnablei(GL_SCISSOR_TEST, static_cast<GLuint>(i));
        glScissorIndexed(static_cast<GLuint>(i), src.x, src.y, src.width, src.height);
      } else {
        glDisablei(GL_SCISSOR_TEST, static_cast<GLuint>(i));
      }
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
  m_cache.viewports.resize(static_cast<size_t>(maxViewports));
}

void ViewportTracker::apply(const ViewportState &state) {

  for (size_t i = 0; i < state.viewports.size(); ++i) {
    const auto &src = state.viewports[i];
    auto &dst = m_cache.viewports[i];
    if (dst != src) {
      dst = src;
      glViewportIndexedf(static_cast<GLuint>(i), src.x, src.y, src.width, src.height);
      glDepthRangeIndexed(static_cast<GLuint>(i), src.minDepth, src.maxDepth);
    }
  }
}