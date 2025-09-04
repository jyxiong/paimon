#pragma once

#include "glad/gl.h"

namespace paimon {

struct DepthBias {
  float constant = 0.0f;
  float slope = 0.0f;

  bool operator==(const DepthBias &other) const = default;
};

struct RasterizationState {
  bool depthClampEnable = false;
  bool rasterizerDiscardEnable = false;
  GLenum polygonMode = GL_FILL;
  GLenum cullMode = GL_BACK;
  bool cullEnable = true;
  GLenum frontFace = GL_CCW;
  bool depthBiasEnable = false;
  DepthBias depthBias;
  float lineWidth = 1.0f;
  float pointSize = 1.0f;
};
} // namespace paimon