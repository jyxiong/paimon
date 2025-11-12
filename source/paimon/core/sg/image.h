#pragma once

#include <string>
#include <vector>

namespace paimon {
namespace sg {

/// Image data for textures
struct Image {
  std::string name;
  std::string uri;       // URI or file path
  std::string mime_type; // e.g., "image/png", "image/jpeg"

  // Raw image data
  std::vector<unsigned char> data;
  int width = 0;
  int height = 0;
  int components = 0; // Number of color components (1-4)
  int bits = 8;       // Bits per component

  Image() = default;
  Image(const std::string &name) : name(name) {}
};

} // namespace sg
} // namespace paimon
