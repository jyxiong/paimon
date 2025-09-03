#pragma once

#include "glad/gl.h"

namespace paimon {
// TODO: support multiple samples
struct MultisampleState {
  bool sampleShadingEnable = false;
  float minSampleShading = 0.0f;
  GLenum sampleMask = GL_NONE;

  bool alphaToCoverageEnable = false;
  bool alphaToOneEnable = false;

  bool operator==(const MultisampleState &other) const noexcept = default;
};
} // namespace paimon