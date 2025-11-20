#pragma once

#include <unordered_map>

#include <glad/gl.h>

#include "paimon/opengl/state.h"
#include "paimon/opengl/program_pipeline.h"
#include "paimon/opengl/shader_program.h"

namespace paimon {

struct GraphicsPipelineCreateInfo {
  // shader stages (optional)
  std::unordered_map<GLbitfield, ShaderProgram*> shaderStages;

  // pipeline state
  PipelineState state;
};

class GraphicsPipeline : public ProgramPipeline {
public:
  GraphicsPipeline(const GraphicsPipelineCreateInfo &ci);

  const PipelineState& getState() const;

private:
  PipelineState m_state;
};
} // namespace paimon