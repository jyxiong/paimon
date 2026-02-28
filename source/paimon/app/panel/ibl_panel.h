#pragma once

#include <memory>
#include <string>

#include "paimon/opengl/texture.h"
#include "paimon/utility/ibl_sampler.h"

namespace paimon {

/// Panel for Image-Based Lighting (IBL) operations.
/// Allows loading an equirectangular HDR map, generating IBL textures via
/// IBLSampler, and wiring the results into a scene Environment entity.
class IBLPanel {
public:
  IBLPanel();
  ~IBLPanel();

  void onImGuiRender();

private:
  /// Load an HDR equirectangular image from disk and upload it as a GL texture.
  bool loadHDR(const std::string &path);

  /// Run IBLSampler on the currently loaded equirectangular texture
  /// and wire the results into the scene Environment entity.
  void generateIBL();

private:
  // The loaded equirectangular texture (used as input to IBLSampler)
  std::unique_ptr<Texture> m_equirectTexture;

  // The IBL sampler that generates irradiance / prefiltered / BRDF LUT maps
  std::unique_ptr<IBLSampler> m_iblSampler;

  // Cached path of the last loaded HDR file
  std::string m_hdrPath;

  // Image display size inside the panel
  float m_previewWidth  = 320.0f;
  float m_previewHeight = 160.0f;
};

} // namespace paimon
