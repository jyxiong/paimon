#pragma once

#include <vector>

#include "glad/gl.h"
#include "glm/vec4.hpp"

namespace paimon {

struct ColorBlendState {
  struct Attachment
  {
    struct BlendFunction {
      GLenum srcRGBFactor = GL_ONE;
      GLenum dstRGBFactor = GL_ZERO; 
      GLenum srcAlphaFactor = GL_ONE;
      GLenum dstAlphaFactor = GL_ZERO;

      bool operator==(const BlendFunction& other) const;
    };

    struct BlendEquation {
      GLenum rgbBlendOp = GL_FUNC_ADD;
      GLenum alphaBlendOp = GL_FUNC_ADD;

      bool operator==(const BlendEquation& other) const;
    };

    bool enabled = false;
    BlendFunction blendFactors;
    BlendEquation blendOp;
  };

  bool logicOpEnable = false;
  GLenum logicOp = GL_COPY;

  glm::vec4 blendConstants = {0.0f, 0.0f, 0.0f, 0.0f};
  std::vector<Attachment> attachments;
};
}