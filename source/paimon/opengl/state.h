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

  void apply(const std::vector<ScissorState> &state);
private:
  std::vector<ScissorState> m_cache;
};  

class StencilTracker {
public:
  StencilTracker();
  ~StencilTracker() = default;

  void apply(const StencilState &state);
private:
  StencilState m_cache;
};

class TessellationTracker {
public:
  TessellationTracker() = default;
  ~TessellationTracker() = default;

  void apply(const TessellationState &state);

private:
  TessellationState m_cache;
};

class ViewportTracker {
public:
  ViewportTracker();
  ~ViewportTracker() = default;

  void apply(const std::vector<ViewportState> &state);
private:
  std::vector<ViewportState> m_cache;
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
  StencilTracker stencil;
  TessellationTracker tessellation;
  ViewportTracker viewport;
};
} // namespace paimon