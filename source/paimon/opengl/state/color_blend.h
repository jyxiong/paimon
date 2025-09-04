#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {
struct BlendFactor {
  GLenum srcRGBFactor = GL_ONE;
  GLenum dstRGBFactor = GL_ZERO;
  GLenum srcAlphaFactor = GL_ONE;
  GLenum dstAlphaFactor = GL_ZERO;

  bool operator==(const BlendFactor &other) const;
};

struct BlendEquation {
  GLenum rgbBlendOp = GL_FUNC_ADD;
  GLenum alphaBlendOp = GL_FUNC_ADD;

  bool operator==(const BlendEquation &other) const;
};

struct ColorWriteMask {
  bool red = true;
  bool green = true;
  bool blue = true;
  bool alpha = true;

  bool operator==(const ColorWriteMask &other) const = default;
};

struct ColorBlendAttachment {
  bool blendEnabled = false;
  BlendFactor blendFactor;
  BlendEquation blendEquation;
  ColorWriteMask colorWriteMask;
};

struct BlendConstants {
  float red = 0.0f;
  float green = 0.0f;
  float blue = 0.0f;
  float alpha = 0.0f;

  bool operator==(const BlendConstants &other) const = default;
};

struct ColorBlendState {

  bool logicOpEnable = false;
  GLenum logicOp = GL_COPY;
  BlendConstants blendConstants;
  std::vector<ColorBlendAttachment> attachments;
};

} // namespace paimon