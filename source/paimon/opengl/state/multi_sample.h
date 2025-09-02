#pragma once

#include "glad/gl.h"

namespace paimon {
struct Multisample
{
  bool enabled = false; // glEnable(GL_MULTISAMPLE)
  bool sampleShadingEnabled = false; // glEnable(GL_SAMPLE_SHADING)
  float minSampleShading = 0.0f; // glMinSampleShading
  GLenum sampleMask = GL_NONE; // glSampleMaski

  bool alphaToCoverageEnable = false;      // glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE)
  bool alphaToOneEnable      = false;      // glEnable(GL_SAMPLE_ALPHA_TO_ONE)

  bool operator==(const Multisample &other) const noexcept = default;
};
} // namespace paimon