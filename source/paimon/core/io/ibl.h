#pragma once

#include <filesystem>
#include <memory>

#include "paimon/core/ecs/scene.h"
#include "paimon/opengl/texture.h"

// Forward declarations
namespace paimon {
class RenderContext;
class EquirectangularToCubemapPass;
class IrradianceMapPass;
class PrefilteredMapPass;
class BRDFLUTPass;
} // namespace paimon

namespace paimon {

/// IBL (Image-Based Lighting) loader.
/// Loads an HDR equirectangular image from disk, pre-computes the IBL textures
/// (irradiance map, pre-filtered map, BRDF LUT), and stores the results in the
/// scene's Environment entity.  If the scene does not yet have an Environment
/// entity one is created automatically.
class IBLLoader {
public:
  IBLLoader(const std::filesystem::path &filepath);
  ~IBLLoader();

  IBLLoader(const IBLLoader &) = delete;
  IBLLoader &operator=(const IBLLoader &) = delete;

  void load(ecs::Scene &scene);

private:
  void loadHDRTexture();
  void processIBL();

private:
  std::filesystem::path m_filepath;

  std::unique_ptr<RenderContext> m_renderContext;

  std::shared_ptr<Texture> m_equirectangularTexture;

  std::unique_ptr<EquirectangularToCubemapPass> m_equirectToCubemapPass;
  std::unique_ptr<IrradianceMapPass> m_irradianceMapPass;
  std::unique_ptr<PrefilteredMapPass> m_prefilteredMapPass;
  std::unique_ptr<BRDFLUTPass> m_brdfLUTPass;
};

} // namespace paimon
