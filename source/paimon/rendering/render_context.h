#pragma once

#include "paimon/rendering/graphics_pipeline.h"

namespace paimon {

class RenderContext {
public:
  void bindGraphicsPipeline(const GraphicsPipeline &pipeline);

private:
  GraphicsPipeline m_currentPipeline;
};
} // namespace paimon