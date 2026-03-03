#include "paimon/core/io/ibl.h"

#include <glad/gl.h>
#include <stb_image.h>
#include <stb_image_write.h>

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

#ifdef PAIMON_DEBUG
  // save((std::filesystem::path(PAIMON_TEXTURE_DIR) / "ibl_output"));
#endif
}

void IBLLoader::save(const std::filesystem::path &directory) {
  // Ensure output directory exists
  std::filesystem::create_directories(directory);

  // Save cubemap faces
  saveCubemap(m_equirectToCubemapPass->getCubemap(),
                     directory / "env_cubemap", defaultCubemapSize, 0);
  saveCubemap(*m_irradianceMapPass->getIrradianceMap(),
                     directory / "irradiance_map", defaultIrradianceSize, 0);
  for (uint32_t mip = 0; mip < defaultPrefilteredMipLevels; ++mip) {
    uint32_t mipSize = defaultPrefilteredSize >> mip;
    saveCubemap(*m_prefilteredMapPass->getPrefilteredMap(),
                       directory / "prefiltered_map", mipSize, mip);
  } 
  // Save BRDF LUT
  save2DTexture(*m_brdfLUTPass->getBRDFLUT(), directory / "brdf_lut.hdr",
                      defaultBRDFLUTSize, defaultBRDFLUTSize);
}

void IBLLoader::loadHDRTexture() {
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

void IBLLoader::saveCubemap(const Texture &cubemap, const std::filesystem::path &basePath,
                             int size, int mipLevel) {
 const char *faceNames[6] = {
      "px", "nx", // +X, -X
      "py", "ny", // +Y, -Y
      "pz", "nz"  // +Z, -Z
  };

  std::vector<float> pixels(size * size * 3);

  for (int face = 0; face < 6; ++face) {
    // For cubemap textures, glGetTextureSubImage treats them as an array of 6
    // slices zoffset is the face index (0-5), depth is the number of faces to
    // access (1)
    glGetTextureSubImage(cubemap.get_name(),
                         mipLevel, // level
                         0, 0,
                         face, // xoffset, yoffset, zoffset (face index: 0-5)
                         size, size, 1, // width, height, depth (1 face)
                         GL_RGB,        // format
                         GL_FLOAT,      // type
                         pixels.size() * sizeof(float), // bufSize
                         pixels.data()                  // pixels
    );

    // Save to HDR file (no tone mapping, preserve full HDR range)
    std::string filename = basePath.string();
    if (mipLevel > 0) {
      filename += "_mip" + std::to_string(mipLevel);
    }
    filename += "_";
    filename += faceNames[face];
    filename += ".hdr";

    stbi_flip_vertically_on_write(1);
    if (!stbi_write_hdr(filename.c_str(), size, size, 3, pixels.data())) {
      LOG_ERROR("  Failed to save: {}", filename);
    }
  }
}

void IBLLoader::save2DTexture(const Texture &texture, const std::filesystem::path &filepath,
                               int width, int height) {
  std::vector<float> pixels(width * height * 2); // RG format

  // Read texture data
  glGetTextureSubImage(texture.get_name(),
                       0,                // level
                       0, 0, 0,          // xoffset, yoffset, zoffset
                       width, height, 1, // width, height, depth
                       GL_RG,            // format (BRDF LUT is RG16F)
                       GL_FLOAT,         // type
                       pixels.size() * sizeof(float), // bufSize
                       pixels.data()                  // pixels
  );

   // Convert RG to RGB for HDR file (add blue channel = 0)
  std::vector<float> pixelsRGB(width * height * 3);
  for (int i = 0; i < width * height; ++i) {
    pixelsRGB[i * 3 + 0] = pixels[i * 2 + 0]; // R
    pixelsRGB[i * 3 + 1] = pixels[i * 2 + 1]; // G
    pixelsRGB[i * 3 + 2] = 0.0f;              // B = 0
  }

  // Check data range
  float maxR = 0.0f, maxG = 0.0f;
  for (int i = 0; i < width * height; ++i) {
    maxR = std::max(maxR, pixels[i * 2 + 0]);
    maxG = std::max(maxG, pixels[i * 2 + 1]);
  }

  stbi_flip_vertically_on_write(1);
  if (!stbi_write_hdr(filepath.string().c_str(), width, height, 3, pixelsRGB.data())) {
    LOG_ERROR("  Failed to save: {}", filepath.string());
  }
}
