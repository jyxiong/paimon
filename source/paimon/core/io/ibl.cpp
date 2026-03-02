#include "paimon/core/io/ibl.h"

#include <glad/gl.h>
#include <stb_image.h>

#include "paimon/core/ecs/components.h"
#include "paimon/core/log_system.h"
#include "paimon/rendering/render_context.h"
#include "paimon/utility/brdf_lut_pass.h"
#include "paimon/utility/equirectangular_to_cubemap_pass.h"
#include "paimon/utility/irradiance_map_pass.h"
#include "paimon/utility/prefiltered_map_pass.h"

using namespace paimon;

namespace {
constexpr uint32_t defaultCubemapSize          = 1024;
constexpr uint32_t defaultIrradianceSize        = 32;
constexpr uint32_t defaultPrefilteredSize       = 512;
constexpr uint32_t defaultPrefilteredMipLevels  = 5;
constexpr uint32_t defaultBRDFLUTSize           = 512;
} // namespace

IBLLoader::IBLLoader(const std::filesystem::path &filepath)
    : m_filepath(filepath),
      m_renderContext(std::make_unique<RenderContext>()),
      m_equirectToCubemapPass(
          std::make_unique<EquirectangularToCubemapPass>(*m_renderContext)),
      m_irradianceMapPass(
          std::make_unique<IrradianceMapPass>(*m_renderContext)),
      m_prefilteredMapPass(
          std::make_unique<PrefilteredMapPass>(*m_renderContext)),
      m_brdfLUTPass(std::make_unique<BRDFLUTPass>(*m_renderContext)) {}

IBLLoader::~IBLLoader() = default;

void IBLLoader::load(ecs::Scene &scene) {
  loadHDRTexture();

  if (!m_equirectangularTexture) {
    LOG_ERROR("IBLLoader: aborting – HDR texture could not be loaded from '{}'",
              m_filepath.string());
    return;
  }

  processIBL();

  // Get the existing Environment entity, or create a new one.
  auto envEntity = scene.getEnvironment();
  if (!envEntity) {
    envEntity = scene.createEntity("Environment");
    envEntity.addComponent<ecs::Environment>();
    scene.setEnvironment(envEntity);
    LOG_INFO("IBLLoader: created new Environment entity");
  }

  auto &envComp = envEntity.getOrAddComponent<ecs::Environment>();
  envComp.equirectangularMap = m_equirectangularTexture;
  envComp.irradianceMap      = m_irradianceMapPass->getIrradianceMap();
  envComp.prefilteredMap     = m_prefilteredMapPass->getPrefilteredMap();
  envComp.brdfLUT            = m_brdfLUTPass->getBRDFLUT();

  LOG_INFO("IBLLoader: environment textures loaded from '{}'",
           m_filepath.string());
}

void IBLLoader::loadHDRTexture() {
  stbi_set_flip_vertically_on_load(true);

  int width = 0, height = 0, nrChannels = 0;
  float *data = stbi_loadf(m_filepath.string().c_str(), &width, &height,
                            &nrChannels, 0);

  if (!data) {
    LOG_ERROR("IBLLoader: failed to load HDR image '{}': {}",
              m_filepath.string(), stbi_failure_reason());
    return;
  }

  GLenum format = GL_RGB;
  if (nrChannels == 1)
    format = GL_RED;
  else if (nrChannels == 4)
    format = GL_RGBA;

  m_equirectangularTexture = std::make_shared<Texture>(GL_TEXTURE_2D);
  m_equirectangularTexture->set_storage_2d(1, GL_RGB32F, width, height);
  m_equirectangularTexture->set_sub_image_2d(0, 0, 0, width, height, format,
                                              GL_FLOAT, data);
  m_equirectangularTexture->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_equirectangularTexture->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  m_equirectangularTexture->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_equirectangularTexture->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
}

void IBLLoader::processIBL() {
  m_equirectToCubemapPass->execute(*m_equirectangularTexture,
                                    defaultCubemapSize);
  m_irradianceMapPass->execute(m_equirectToCubemapPass->getCubemap(),
                                defaultIrradianceSize);
  m_prefilteredMapPass->execute(m_equirectToCubemapPass->getCubemap(),
                                 defaultPrefilteredSize,
                                 defaultPrefilteredMipLevels);
  m_brdfLUTPass->execute(defaultBRDFLUTSize);
}
