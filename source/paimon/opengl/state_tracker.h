#pragma once

#include "paimon/opengl/state/color_blend.h"
#include "paimon/opengl/state/depth.h"
#include "paimon/opengl/state/input_assembly.h"
#include "paimon/opengl/state/multisample.h"
#include "paimon/opengl/state/rasterization.h"
#include "paimon/opengl/state/scissor.h"
#include "paimon/opengl/state/tessellation.h"
#include "paimon/opengl/state/vertex_input.h"
#include "paimon/opengl/state/viewport.h"

namespace paimon {

class ColorBlendTracker
{
public:
  ColorBlendTracker();
  ~ColorBlendTracker() = default;

  void apply(const ColorBlendState &state);

private:
  ColorBlendState m_cache;
};

class DepthTracker
{
public:
  DepthTracker() = default;
  ~DepthTracker() = default;

  void apply(const DepthState &state);
private:
  DepthState m_cache;
};

class StateTracker {
public:
  StateTracker() = default;
  ~StateTracker() = default;

private:
};

class InputAssemblyTracker {
public:
  InputAssemblyTracker() = default;
  ~InputAssemblyTracker() = default;

  void apply(const InputAssemblyState &state);
private:
  InputAssemblyState m_cache;
};

class MultisampleTracker {
public:
  MultisampleTracker();
  ~MultisampleTracker() = default;

  void apply(const MultisampleState &state);
private:
  MultisampleState m_cache;
};

class RasterizationTracker {
public:

  RasterizationTracker() = default;
  ~RasterizationTracker() = default;

  void apply(const RasterizationState &state);
private:
  RasterizationState m_cache;
};  

class ScissorTracker {
public:
  ScissorTracker();
  ~ScissorTracker() = default;

  void apply(const ScissorState &state);
private:
  ScissorState m_cache;
};  

class TessellationTracker {
public:
  TessellationTracker() = default;
  ~TessellationTracker() = default;

  void apply(const TessellationState &state);

private:
  TessellationState m_cache;
};

class VertexInputTracker {
public:
  VertexInputTracker();
  ~VertexInputTracker() = default;

  void apply(const VertexInputState &state);
private:
  VertexInputState m_cache;
};

class ViewportTracker {
public:
  ViewportTracker();
  ~ViewportTracker() = default;

  void apply(const ViewportState &state);
private:
  ViewportState m_cache;
};

class PipelineTracker {
public:
  PipelineTracker() = default;
  ~PipelineTracker() = default;

  // TODO: track program pipeline

  ColorBlendTracker colorBlend;
  DepthTracker depth;
  InputAssemblyTracker inputAssembly;
  MultisampleTracker multisample;
  RasterizationTracker rasterization;
  ScissorTracker scissor;
  TessellationTracker tessellation;
  // TODO: track VAO instead of vertex input
  VertexInputTracker vertexInput;
  ViewportTracker viewport;
};
} // namespace paimon 