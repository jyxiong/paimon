#pragma once

#include <cstdint>
#include <memory>
#include <filesystem>

#include "paimon/core/sg/image.h"

namespace paimon {

class Image {
public:
  static sg::Image load(const std::filesystem::path &filepath, int desired_channels = 0);
};

} // namespace paimon