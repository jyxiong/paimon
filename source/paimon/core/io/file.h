#pragma once

#include <filesystem>
#include <vector>

namespace paimon {
class File {
public:
  static std::string readText(const std::filesystem::path &path);

  static std::vector<uint8_t> readBinary(const std::filesystem::path &path);

  static void writeText(const std::filesystem::path &path,
                        const std::string &content);
};
} // namespace paimon