#pragma once

#include <memory>

#include "paimon/opengl/texture.h"
#include "paimon/opengl/sampler.h"

namespace paimon {
namespace sg {

/// Texture combining OpenGL texture and sampler
struct Texture {
  // OpenGL texture and sampler objects
  std::shared_ptr<paimon::Texture> image = nullptr;
  std::shared_ptr<paimon::Sampler> sampler = nullptr;

};

} // namespace sg
} // namespace paimon
