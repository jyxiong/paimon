#pragma once

#include "glad/gl.h"

namespace paimon {
struct Multisample {
  bool sampleShadingEnabled = false;
  float minSampleShading = 0.0f;
  GLenum sampleMask = GL_NONE;

  bool alphaToCoverageEnable = false;
  bool alphaToOneEnable = false;

  bool operator==(const Multisample &other) const noexcept = default;
};
} // namespace paimon