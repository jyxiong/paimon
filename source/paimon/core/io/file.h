#pragma once

#include <filesystem>

namespace paimon {
class File {
public:
  static std::string readText(const std::filesystem::path &path);

  static std::vector<uint8_t> readBinary(const std::filesystem::path &path);
};
} // namespace paimon