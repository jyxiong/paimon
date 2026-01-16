#include "paimon/utility/ibl_sampler.h"

#include <filesystem>
#include <stb_image.h>
#include <stb_image_write.h>

#include "paimon/config.h"
#include "paimon/opengl/texture.h"
#include "paimon/rendering/render_context.h"
#include "paimon/utility/brdf_lut_pass.h"
#include "paimon/utility/equirectangular_to_cubemap_pass.h"
#include "paimon/utility/irradiance_map_pass.h"
#include "paimon/utility/prefiltered_map_pass.h"
#include "paimon/core/log_system.h"

using namespace paimon;

std::unique_ptr<Texture> loadHDRTexture(const std::string &path);
void saveCubemapToFiles(const Texture &cubemap, const std::string &basePath,
                        int size, int mipLevel = 0);
void save2DTextureToFile(const Texture &texture, const std::string &path,
                         int width, int height);

constexpr uint32_t defaultCubemapSize = 1024;
constexpr uint32_t defaultIrradianceSize = 32;
constexpr uint32_t defaultPrefilteredSize = 512;
constexpr uint32_t defaultPrefilteredMipLevels = 5;
constexpr uint32_t defaultBRDFLUTSize = 512;

IBLSampler::IBLSampler()
    : m_renderContext(std::make_unique<RenderContext>()),
      m_equirectToCubemapPass(
          std::make_unique<EquirectangularToCubemapPass>(*m_renderContext)),
      m_irradianceMapPass(
          std::make_unique<IrradianceMapPass>(*m_renderContext)),
      m_prefilteredMapPass(
          std::make_unique<PrefilteredMapPass>(*m_renderContext)),
      m_brdfLUTPass(std::make_unique<BRDFLUTPass>(*m_renderContext)) {}

IBLSampler::~IBLSampler() = default;

void IBLSampler::execute(const Texture &equirectangular) {
  m_equirectToCubemapPass->execute(equirectangular, defaultCubemapSize);
  m_irradianceMapPass->execute(m_equirectToCubemapPass->getCubemap(),
                               defaultIrradianceSize);
  m_prefilteredMapPass->execute(m_equirectToCubemapPass->getCubemap(),
                                defaultPrefilteredSize,
                                defaultPrefilteredMipLevels);
  m_brdfLUTPass->execute(defaultBRDFLUTSize);
}

void IBLSampler::save() {
  // Save cubemap faces
  saveCubemapToFiles(m_equirectToCubemapPass->getCubemap(),
                     std::filesystem::path(PAIMON_TEXTURE_DIR) / "env_cubemap", defaultCubemapSize, 0);
  saveCubemapToFiles(m_irradianceMapPass->getIrradianceMap(),
                     std::filesystem::path(PAIMON_TEXTURE_DIR) / "irradiance_map", defaultIrradianceSize, 0);

  for (uint32_t mip = 0; mip < defaultPrefilteredMipLevels; ++mip) {
    uint32_t mipSize = defaultPrefilteredSize >> mip;
    saveCubemapToFiles(m_prefilteredMapPass->getPrefilteredMap(),
                       std::filesystem::path(PAIMON_TEXTURE_DIR) / "prefiltered_map", mipSize, mip);
  }

  // Save BRDF LUT
  save2DTextureToFile(m_brdfLUTPass->getBRDFLUT(), std::filesystem::path(PAIMON_TEXTURE_DIR) / "brdf_lut.hdr",
                      defaultBRDFLUTSize, defaultBRDFLUTSize);
}

// Save cubemap faces to HDR files
void saveCubemapToFiles(const Texture &cubemap, const std::string &basePath,
                        int size, int mipLevel) {
  const char *faceNames[6] = {
      "px", "nx", // +X, -X
      "py", "ny", // +Y, -Y
      "pz", "nz"  // +Z, -Z
  };

  std::vector<float> pixels(size * size * 3);

  // Ensure all GPU operations are complete

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
    std::string filename = basePath;
    if (mipLevel > 0) {
      filename += "_mip" + std::to_string(mipLevel);
    }
    filename += "_";
    filename += faceNames[face];
    filename += ".hdr";

    if (!stbi_write_hdr(filename.c_str(), size, size, 3, pixels.data())) {
      LOG_ERROR("  Failed to save: {}", filename);
    }
  }
}

// Save 2D texture to HDR file
void save2DTextureToFile(const Texture &texture, const std::string &filepath,
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

  if (!stbi_write_hdr(filepath.c_str(), width, height, 3, pixelsRGB.data())) {
    LOG_ERROR("  Failed to save: {}", filepath);
  }
}