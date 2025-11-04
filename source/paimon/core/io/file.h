#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace paimon {

class File {
public:
  static std::optional<std::vector<uint8_t>> readBinary(const std::filesystem::path& filepath);
  
  static std::optional<std::string> readText(const std::filesystem::path& filepath);
};

} // namespace paimon