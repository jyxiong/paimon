#pragma once

#include "paimon/opengl/state/color_blend.h"
#include "paimon/opengl/state/depth.h"
#include "paimon/opengl/state/input_assembly.h"
#include "paimon/opengl/state/multisample.h"
#include "paimon/opengl/state/rasterization.h"
#include "paimon/opengl/state/scissor.h"
#include "paimon/opengl/state/stencil.h"
#include "paimon/opengl/state/tessellation.h"
#include "paimon/opengl/state/viewport.h"

namespace paimon {

struct GraphicsPipeline {
  ColorBlendState colorBlendState;
  DepthState depthState;
  InputAssemblyState inputAssemblyState;
  MultisampleState multisampleState;
  RasterizationState rasterizationState;
  std::vector<ScissorState> scissorStates;
  StencilState stencilState;
  TessellationState tessellationState;
  std::vector<ViewportState> viewportStates;
};
} // namespace paimon