#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <stb_image.h>
#include <stb_image_write.h>

#include "paimon/app/application.h"
#include "paimon/config.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/texture.h"
#include "paimon/utility/ibl_sampler.h"

using namespace paimon;

// Window dimensions
const int WIDTH = 800;
const int HEIGHT = 600;

// Load HDR texture from file
std::unique_ptr<Texture> loadHDRTexture(const std::string &path) {
  stbi_set_flip_vertically_on_load(true);

  int width, height, nrComponents;
  float *data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);

  if (!data) {
    std::cerr << "Failed to load HDR image: " << path << std::endl;
    return nullptr;
  }

  auto texture = std::make_unique<Texture>(GL_TEXTURE_2D);

  GLenum format = GL_RGB;
  if (nrComponents == 1)
    format = GL_RED;
  else if (nrComponents == 3)
    format = GL_RGB;
  else if (nrComponents == 4)
    format = GL_RGBA;

  texture->set_storage_2d(1, GL_RGB32F, width, height);
  texture->set_sub_image_2d(0, 0, 0, width, height, format, GL_FLOAT, data);

  texture->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  texture->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  texture->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);

  return texture;
}

class IBLApp : public Application {
public:
  IBLApp() : Application() {}

  ~IBLApp() override = default;
};

int main() {

  LogSystem::init();

  IBLApp app;

  // Load HDR environment map
  auto hdrTexture = loadHDRTexture(std::filesystem::path(PAIMON_TEXTURE_DIR) /
                                   "belfast_sunset_puresky_2k.hdr");

  // Create IBL sampler
  IBLSampler iblSampler;
  iblSampler.execute(*hdrTexture);
  iblSampler.save();

  return 0;
}
