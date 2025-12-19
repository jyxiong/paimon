#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>

#include "paimon/core/sg/image.h"

namespace paimon {

class ImageLoader {
public:
  static sg::Image load(const std::filesystem::path &filepath,
                        int desired_channels = 0);
};

} // namespace paimon