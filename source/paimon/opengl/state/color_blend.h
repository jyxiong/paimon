#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {

struct ColorBlend {
  struct Attachment
  {
    bool enabled = false; // glEnablei(GL_BLEND, index)
    GLenum srcRGBFactor = GL_ONE; // glBlendFuncSeparatei(index, srcRGB
    GLenum dstRGBFactor = GL_ZERO; // dstRGB)
    GLenum rgbBlendOp = GL_FUNC_ADD; // glBlendEquationSeparatei(index, modeRGB)
    GLenum srcAlphaFactor = GL_ONE; // glBlendFuncSeparatei(index, srcAlpha
    GLenum dstAlphaFactor = GL_ZERO; // dstAlpha)
    GLenum alphaBlendOp = GL_FUNC_ADD; // glBlendEquationSeparatei(index, mode
    

    bool operator==(const Attachment &other) const = default;
  };

  bool logicOpEnable = false; // glEnable(GL_COLOR_LOGIC_OP)
  GLenum logicOp = GL_COPY; // glLogicOp(logicOp)

  float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // glBlendColor
  std::vector<Attachment> attachments; // glBlendFuncSeparatei + glBlendEquationSeparatei
};
}