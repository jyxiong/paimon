#pragma once

#include "paimon/opengl/state/color_blend.h"
#include "paimon/opengl/state/depth_stencil.h"
#include "paimon/opengl/state/input_assembly.h"
#include "paimon/opengl/state/multisample.h"
#include "paimon/opengl/state/rasterization.h"
#include "paimon/opengl/state/tessellation.h"
#include "paimon/opengl/state/viewport.h"

namespace paimon {

struct PipelineState {
  ColorBlendState colorBlend;
  DepthStencilState depthStencil;
  InputAssemblyState inputAssembly;
  MultisampleState multisample;
  RasterizationState rasterization;
  TessellationState tessellation;
  ViewportState viewport;

  void apply(const PipelineState &state);

private:
  void apply(const ColorBlendState &state);
  void apply(const DepthStencilState &state);
  void apply(const InputAssemblyState &state);
  void apply(const MultisampleState &state);
  void apply(const RasterizationState &state);
  void apply(const TessellationState &state);
  void apply(const ViewportState &state);
};

} // namespace paimon