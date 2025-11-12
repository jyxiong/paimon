#pragma once

#include <string>

namespace paimon {
namespace sg {

/// Texture sampler parameters
struct Sampler {

  // Texture filtering
  enum class MinFilter {
    Nearest = 9728,
    Linear = 9729,
    NearestMipmapNearest = 9984,
    LinearMipmapNearest = 9985,
    NearestMipmapLinear = 9986,
    LinearMipmapLinear = 9987
  };

  enum class MagFilter { Nearest = 9728, Linear = 9729 };

  // Texture wrapping
  enum class WrapMode {
    ClampToEdge = 33071,
    MirroredRepeat = 33648,
    Repeat = 10497
  };

  std::string name;
  MinFilter min_filter = MinFilter::Linear;
  MagFilter mag_filter = MagFilter::Linear;
  WrapMode wrap_s = WrapMode::Repeat; // U coordinate
  WrapMode wrap_t = WrapMode::Repeat; // V coordinate

  Sampler() = default;
};

} // namespace sg
} // namespace paimon
