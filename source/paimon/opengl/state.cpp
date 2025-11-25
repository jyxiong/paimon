#include "paimon/opengl/state.h"

using namespace paimon;

void PipelineState::apply(const PipelineState &state) {
  apply(state.colorBlend);
  apply(state.depthStencil);
  apply(state.inputAssembly);
  apply(state.multisample);
  apply(state.rasterization);
  apply(state.tessellation);
  apply(state.viewport);
}

void PipelineState::apply(const ColorBlendState &state) {
  // Logic operation
  if (colorBlend.logicOpEnable != state.logicOpEnable) {
    colorBlend.logicOpEnable = state.logicOpEnable;
    if (state.logicOpEnable) {
      glEnable(GL_COLOR_LOGIC_OP);
    } else {
      glDisable(GL_COLOR_LOGIC_OP);
    }
  }

  if (colorBlend.logicOp != state.logicOp) {
    colorBlend.logicOp = state.logicOp;
    glLogicOp(state.logicOp);
  }

  // Blend constants
  bool constantsChanged = false;
  for (int i = 0; i < 4; ++i) {
    if (colorBlend.blendConstants[i] != state.blendConstants[i]) {
      constantsChanged = true;
      break;
    }
  }
  if (constantsChanged) {
    std::copy(std::begin(state.blendConstants), std::end(state.blendConstants),
              std::begin(colorBlend.blendConstants));
    glBlendColor(state.blendConstants[0], state.blendConstants[1],
                 state.blendConstants[2], state.blendConstants[3]);
  }

  // Per-attachment blend state
  for (size_t i = 0; i < state.attachments.size(); ++i) {
    const auto &src = state.attachments[i];
    auto &dst = colorBlend.attachments[i];

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

void PipelineState::apply(const DepthStencilState &state) {
  // Depth test
  if (depthStencil.depthTestEnable != state.depthTestEnable) {
    depthStencil.depthTestEnable = state.depthTestEnable;
    if (state.depthTestEnable) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  // Depth write
  if (depthStencil.depthWriteEnable != state.depthWriteEnable) {
    depthStencil.depthWriteEnable = state.depthWriteEnable;
    glDepthMask(state.depthWriteEnable ? GL_TRUE : GL_FALSE);
  }

  // Depth compare operation
  if (depthStencil.depthCompareOp != state.depthCompareOp) {
    depthStencil.depthCompareOp = state.depthCompareOp;
    glDepthFunc(state.depthCompareOp);
  }

  // Stencil test
  if (depthStencil.stencilTestEnable != state.stencilTestEnable) {
    depthStencil.stencilTestEnable = state.stencilTestEnable;
    if (state.stencilTestEnable) {
      glEnable(GL_STENCIL_TEST);
    } else {
      glDisable(GL_STENCIL_TEST);
    }
  }

  // Front face stencil
  if (depthStencil.front != state.front) {
    depthStencil.front = state.front;
    glStencilFuncSeparate(GL_FRONT, state.front.compareOp, 
                         state.front.reference, state.front.compareMask);
    glStencilOpSeparate(GL_FRONT, state.front.failOp, 
                       state.front.depthFailOp, state.front.passOp);
    glStencilMaskSeparate(GL_FRONT, state.front.writeMask);
  }

  // Back face stencil
  if (depthStencil.back != state.back) {
    depthStencil.back = state.back;
    glStencilFuncSeparate(GL_BACK, state.back.compareOp,
                         state.back.reference, state.back.compareMask);
    glStencilOpSeparate(GL_BACK, state.back.failOp,
                       state.back.depthFailOp, state.back.passOp);
    glStencilMaskSeparate(GL_BACK, state.back.writeMask);
  }
}

void PipelineState::apply(const InputAssemblyState &state) {
  // Primitive topology (stored but applied in draw call)
  if (inputAssembly.topology != state.topology) {
    inputAssembly.topology = state.topology;
  }

  // Primitive restart
  if (inputAssembly.primitiveRestartEnable != state.primitiveRestartEnable) {
    inputAssembly.primitiveRestartEnable = state.primitiveRestartEnable;
    if (state.primitiveRestartEnable) {
      glEnable(GL_PRIMITIVE_RESTART);
    } else {
      glDisable(GL_PRIMITIVE_RESTART);
    }
  }
}

void PipelineState::apply(const MultisampleState &state) {
  // Sample shading
  if (multisample.sampleShadingEnable != state.sampleShadingEnable) {
    multisample.sampleShadingEnable = state.sampleShadingEnable;
    if (state.sampleShadingEnable) {
      glEnable(GL_SAMPLE_SHADING);
    } else {
      glDisable(GL_SAMPLE_SHADING);
    }
  }

  if (multisample.minSampleShading != state.minSampleShading) {
    multisample.minSampleShading = state.minSampleShading;
    glMinSampleShading(state.minSampleShading);
  }

  // Sample mask
  for (size_t i = 0; i < state.sampleMask.size(); ++i) {
    if (multisample.sampleMask[i] != state.sampleMask[i]) {
      multisample.sampleMask[i] = state.sampleMask[i];
      glSampleMaski(static_cast<GLuint>(i), state.sampleMask[i]);
    }
  }

  // Alpha to coverage
  if (multisample.alphaToCoverageEnable != state.alphaToCoverageEnable) {
    multisample.alphaToCoverageEnable = state.alphaToCoverageEnable;
    if (state.alphaToCoverageEnable) {
      glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    } else {
      glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
  }

  // Alpha to one
  if (multisample.alphaToOneEnable != state.alphaToOneEnable) {
    multisample.alphaToOneEnable = state.alphaToOneEnable;
    if (state.alphaToOneEnable) {
      glEnable(GL_SAMPLE_ALPHA_TO_ONE);
    } else {
      glDisable(GL_SAMPLE_ALPHA_TO_ONE);
    }
  }
}

void PipelineState::apply(const RasterizationState &state) {
  // Depth clamp
  if (rasterization.depthClampEnable != state.depthClampEnable) {
    rasterization.depthClampEnable = state.depthClampEnable;
    if (state.depthClampEnable) {
      glEnable(GL_DEPTH_CLAMP);
    } else {
      glDisable(GL_DEPTH_CLAMP);
    }
  }

  // Rasterizer discard
  if (rasterization.rasterizerDiscardEnable != state.rasterizerDiscardEnable) {
    rasterization.rasterizerDiscardEnable = state.rasterizerDiscardEnable;
    if (state.rasterizerDiscardEnable) {
      glEnable(GL_RASTERIZER_DISCARD);
    } else {
      glDisable(GL_RASTERIZER_DISCARD);
    }
  }

  // Polygon mode
  if (rasterization.polygonMode != state.polygonMode) {
    rasterization.polygonMode = state.polygonMode;
    glPolygonMode(GL_FRONT_AND_BACK, state.polygonMode);
  }

  // Cull mode
  if (rasterization.cullMode != state.cullMode) {
    rasterization.cullMode = state.cullMode;
    if (state.cullMode == GL_NONE) {
      glDisable(GL_CULL_FACE);
    } else {
      glEnable(GL_CULL_FACE);
      glCullFace(state.cullMode);
    }
  }

  // Front face
  if (rasterization.frontFace != state.frontFace) {
    rasterization.frontFace = state.frontFace;
    glFrontFace(state.frontFace);
  }

  // Depth bias
  if (rasterization.depthBiasEnable != state.depthBiasEnable) {
    rasterization.depthBiasEnable = state.depthBiasEnable;
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

  if (rasterization.depthBiasConstantFactor != state.depthBiasConstantFactor ||
      rasterization.depthBiasSlopeFactor != state.depthBiasSlopeFactor) {
    rasterization.depthBiasConstantFactor = state.depthBiasConstantFactor;
    rasterization.depthBiasSlopeFactor = state.depthBiasSlopeFactor;
    glPolygonOffset(state.depthBiasSlopeFactor, state.depthBiasConstantFactor);
  }

  // Line width
  if (rasterization.lineWidth != state.lineWidth) {
    rasterization.lineWidth = state.lineWidth;
    glLineWidth(state.lineWidth);
  }

  // Point size
  if (rasterization.pointSize != state.pointSize) {
    rasterization.pointSize = state.pointSize;
    glPointSize(state.pointSize);
  }

  // Program point size
  if (rasterization.programPointSize != state.programPointSize) {
    rasterization.programPointSize = state.programPointSize;
    if (state.programPointSize) {
      glEnable(GL_PROGRAM_POINT_SIZE);
    } else {
      glDisable(GL_PROGRAM_POINT_SIZE);
    }
  }
}

void PipelineState::apply(const TessellationState &state) {
  if (tessellation.patchControlPoints != state.patchControlPoints) {
    tessellation.patchControlPoints = state.patchControlPoints;
    glPatchParameteri(GL_PATCH_VERTICES, state.patchControlPoints);
  }
}

void PipelineState::apply(const ViewportState &state) {
  // Apply viewports
  for (size_t i = 0; i < state.viewports.size() && i < viewport.viewports.size(); ++i) {
    const auto &src = state.viewports[i];
    auto &dst = viewport.viewports[i];

    if (dst != src) {
      dst = src;
      glViewportIndexedf(static_cast<GLuint>(i), src.x, src.y,
                         src.width, src.height);
      glDepthRangeIndexed(static_cast<GLuint>(i), src.minDepth, src.maxDepth);
    }
  }

  // Apply scissors
  for (size_t i = 0; i < state.scissors.size() && i < viewport.scissors.size(); ++i) {
    const auto &src = state.scissors[i];
    auto &dst = viewport.scissors[i];

    if (dst != src) {
      dst = src;
      glScissorIndexed(static_cast<GLuint>(i), src.x, src.y,
                       src.width, src.height);
    }
  }
}