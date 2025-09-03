#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {

struct ColorBlendState {
  struct Attachment
  {
    bool enabled = false;
    GLenum srcRGBFactor = GL_ONE;
    GLenum dstRGBFactor = GL_ZERO; 
    GLenum rgbBlendOp = GL_FUNC_ADD;
    GLenum srcAlphaFactor = GL_ONE;
    GLenum dstAlphaFactor = GL_ZERO; 
    GLenum alphaBlendOp = GL_FUNC_ADD;
    

    bool operator==(const Attachment &other) const = default;
  };

  bool logicOpEnable = false;
  GLenum logicOp = GL_COPY;

  float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  std::vector<Attachment> attachments;
};
}