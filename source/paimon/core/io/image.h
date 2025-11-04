#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace paimon {

struct ImageData {
  int width;
  int height;
  std::unique_ptr<uint8_t[], void(*)(void*)> data;

  ImageData() 
    : data(nullptr, [](void*) {}), width(0), height(0) {}
};

class Image {
public:
  static ImageData load(const std::string &filepath, int desired_channels = 0);
};

} // namespace paimon