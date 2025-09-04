#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {
struct MultisampleState {
  bool sampleShadingEnable = false;
  float minSampleShading = 0.0f;
  std::vector<GLenum> sampleMasks;
  bool alphaToCoverageEnable = false;
  bool alphaToOneEnable = false;
};
} // namespace paimon