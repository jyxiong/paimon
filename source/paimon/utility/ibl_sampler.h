#pragma once

#include <memory>
#include "paimon/opengl/texture.h"

namespace paimon {

class RenderContext;
class EquirectangularToCubemapPass;
class IrradianceMapPass;
class PrefilteredMapPass;
class BRDFLUTPass;

/// IBL (Image-Based Lighting) sampler for pre-computing environment maps
class IBLSampler {
public:
  IBLSampler();
  ~IBLSampler();

  IBLSampler(const IBLSampler &other) = delete;
  IBLSampler &operator=(const IBLSampler &other) = delete;

  void execute(const Texture &equirectangular);

  void save();

private:
  std::unique_ptr<RenderContext> m_renderContext;

  std::unique_ptr<Texture> m_equirectangularTexture;
  
  std::unique_ptr<EquirectangularToCubemapPass> m_equirectToCubemapPass;
  std::unique_ptr<IrradianceMapPass> m_irradianceMapPass;
  std::unique_ptr<PrefilteredMapPass> m_prefilteredMapPass;
  std::unique_ptr<BRDFLUTPass> m_brdfLUTPass;
};

} // namespace paimon