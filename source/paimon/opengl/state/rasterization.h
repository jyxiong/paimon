#pragma once

#include "glad/gl.h"

namespace paimon {

// Similar to VkPipelineRasterizationStateCreateInfo
struct RasterizationState {
  // Depth clamp
  bool depthClampEnable = false;
  
  // Rasterizer discard
  bool rasterizerDiscardEnable = false;
  
  // Polygon mode
  GLenum polygonMode = GL_FILL; // GL_FILL, GL_LINE, GL_POINT
  
  // Culling
  GLenum cullMode = GL_BACK; // GL_NONE, GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
  GLenum frontFace = GL_CCW; // GL_CW, GL_CCW
  
  // Depth bias
  bool depthBiasEnable = false;
  float depthBiasConstantFactor = 0.0f;
  float depthBiasClamp = 0.0f;
  float depthBiasSlopeFactor = 0.0f;
  
  // Line width
  float lineWidth = 1.0f;
  
  // Point size (OpenGL specific)
  float pointSize = 1.0f;
  bool programPointSize = false; // Enable gl_PointSize in shader

  bool operator==(const RasterizationState &other) const = default;
};

} // namespace paimon