#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {

// Similar to VkPipelineMultisampleStateCreateInfo
struct MultisampleState {
  // Rasterization samples
  GLenum rasterizationSamples = GL_SAMPLE_BUFFERS; // Sample count
  
  // Sample shading
  bool sampleShadingEnable = false;
  float minSampleShading = 1.0f; // Min fraction for sample shading
  
  // Sample mask
  std::vector<GLuint> sampleMask;
  
  // Alpha to coverage
  bool alphaToCoverageEnable = false;
  
  // Alpha to one
  bool alphaToOneEnable = false;

  MultisampleState() {
    int maxSampleMaskWords = 0;
    glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &maxSampleMaskWords);
    sampleMask.resize(static_cast<size_t>(maxSampleMaskWords));
  }

  bool operator==(const MultisampleState &other) const = default;
};

} // namespace paimon