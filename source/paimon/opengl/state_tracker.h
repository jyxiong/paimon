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
  ColorBlendTracker() = default;
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

class InputAssembllyTracker {
public:
  InputAssembllyTracker() = default;
  ~InputAssembllyTracker() = default;

  void apply(const InputAssemblyState &state);
private:
  InputAssemblyState m_cache;
};

class MultisampleTracker {
public:
  MultisampleTracker() = default;
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
  ScissorTracker() = default;
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
  VertexInputTracker() = default;
  ~VertexInputTracker() = default;

  void apply(const VertexInputState &state);
private:
  VertexInputState m_cache;
};

class ViewportTracker {
public:
  ViewportTracker() = default;
  ~ViewportTracker() = default;

  void apply(const ViewportState &state);
private:
  ViewportState m_cache;
};
} // namespace paimon 