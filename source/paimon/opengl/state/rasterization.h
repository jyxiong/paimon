#pragma once

#include "glad/gl.h"

namespace paimon {
struct RasterizationState
{
  bool depthClampEnable = false;
  GLenum polygonMode = GL_FILL;
  GLenum cullMode = GL_BACK;
  GLenum frontFace = GL_CCW;
  bool depthBiasEnable = false;
  float depthBiasConstantFactor = 0.0f;
  float depthBiasSlopeFactor = 0.0f;
  float lineWidth = 1.0f;
  float pointSize = 1.0f;
  
};
} // namespace paimon