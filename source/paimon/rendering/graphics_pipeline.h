#pragma once

#include "paimon/opengl/state/color_blend.h"
#include "paimon/opengl/state/depth_stencil.h"
#include "paimon/opengl/state/input_assembly.h"
#include "paimon/opengl/state/multisample.h"
#include "paimon/opengl/state/rasterization.h"
#include "paimon/opengl/state/tessellation.h"
#include "paimon/opengl/state/vertex_input.h"
#include "paimon/opengl/state/viewport.h"

namespace paimon {

// Similar to VkGraphicsPipelineCreateInfo
struct GraphicsPipeline {
  ColorBlendState colorBlendState;
  DepthStencilState depthStencilState;
  InputAssemblyState inputAssemblyState;
  MultisampleState multisampleState;
  RasterizationState rasterizationState;
  TessellationState tessellationState;
  VertexInputState vertexInputState;
  ViewportState viewportState;  // Contains both viewports and scissors
  
  // Backward compatibility aliases
  ColorBlendState& colorBlend = colorBlendState;
  DepthStencilState& depthState = depthStencilState;
  DepthStencilState& stencilState = depthStencilState;
  InputAssemblyState& inputAssembly = inputAssemblyState;
  MultisampleState& multisample = multisampleState;
  RasterizationState& rasterization = rasterizationState;
  TessellationState& tessellation = tessellationState;
  VertexInputState& vertexInput = vertexInputState;
  ViewportState& viewport = viewportState;
};

} // namespace paimon