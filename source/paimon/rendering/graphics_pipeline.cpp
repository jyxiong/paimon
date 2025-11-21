#include "paimon/rendering/graphics_pipeline.h"

#include "paimon/opengl/program_pipeline.h"

using namespace paimon;

GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineCreateInfo &ci)
    : ProgramPipeline() {
  for (const auto &[stage, shaderProgram] : ci.shaderStages) {
    use_program_stages(stage, *shaderProgram);
  }

  if (!validate()) {
  }

  m_state = ci.state;
}

const PipelineState &GraphicsPipeline::getState() const { return m_state; }